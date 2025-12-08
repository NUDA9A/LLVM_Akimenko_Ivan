#include "../SDL/sim.h"
#include "LifeLangBaseVisitor.h"
#include "LifeLangLexer.h"
#include "LifeLangParser.h"
#include "antlr4-runtime.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>


struct VarInfo {
  llvm::Value *val_ptr;
  bool is_scalar;
  int dimX, dimY;
};

struct TreeLLVMWalker : public LifeLangBaseVisitor {
  llvm::LLVMContext *ctx;
  llvm::Module *module;
  llvm::IRBuilder<> *builder;

  llvm::Function *currFunc{nullptr};

  llvm::Type* int32Ty;
  llvm::Type* voidTy;
  llvm::Type* int32PtrTy;

  std::vector<std::unordered_map<std::string, VarInfo>> varScopes;

  TreeLLVMWalker(llvm::LLVMContext *context, llvm::IRBuilder<> *irBuilder,
                 llvm::Module *mod)
      : ctx(context), module(mod), builder(irBuilder) {
    int32Ty = llvm::Type::getInt32Ty(*ctx);
    voidTy = llvm::Type::getVoidTy(*ctx);
    int32PtrTy = llvm::PointerType::getUnqual(int32Ty);
  }

  void pushScope() {
    varScopes.emplace_back();
  }

  void popScope() {
    varScopes.pop_back();
  }

  VarInfo* lookUpVar(const std::string& name) {
    for (auto it = varScopes.rbegin(); it != varScopes.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return &found->second;
      }
    }
    return nullptr;
  }

  VarInfo& defineVar(const std::string& name, const VarInfo& info) {
    auto& scope = varScopes.back();
    auto [it, ok] = scope.emplace(name, info);
    return it->second;
  }

  void registerBuiltins() {
    using namespace llvm;

    auto* whiteConst = ConstantInt::get(int32Ty, 0x00FFFFFF);
    auto* whiteGlob = new GlobalVariable(*module, int32Ty, true, GlobalValue::PrivateLinkage, whiteConst, "WHITE");

    VarInfo whiteInfo{};
    whiteInfo.val_ptr = whiteGlob;
    whiteInfo.is_scalar = true;
    whiteInfo.dimX = whiteInfo.dimY = 0;
    defineVar("WHITE", whiteInfo);

    auto* randTy = FunctionType::get(int32Ty, false);
    module->getOrInsertFunction("rand", randTy);

    ArrayRef<Type*> putPixelArgsTy = {int32Ty, int32Ty, int32Ty};
    FunctionType* putPixelType = FunctionType::get(voidTy, putPixelArgsTy, false);
    module->getOrInsertFunction("put_pixel", putPixelType);

    auto* flushTy = FunctionType::get(voidTy, false);
    module->getOrInsertFunction("flush", flushTy);
  }

  llvm::Value* toBool(llvm::Value* v) {
    using namespace llvm;

    Type* ty = v->getType();

    if (ty->isIntegerTy()) {
      if (ty->getIntegerBitWidth() == 1) {
        return v;
      }
      auto* zero = ConstantInt::get(ty, 0);
      return builder->CreateICmpNE(v, zero);
    }

    if (ty->isPointerTy()) {
      auto* nullPtr = ConstantPointerNull::get(cast<PointerType>(ty));
      return builder->CreateICmpNE(v, nullPtr);
    }

    errs() << "[WARN] toBool on unsupported type: ";
    ty->print(errs());
    errs() << "\n";
    return ConstantInt::get(Type::getInt1Ty(*ctx), 1);
  }

  llvm::Value* boolToInt(llvm::Value* v) {
    return builder->CreateZExt(v, int32Ty);
  }

  llvm::Value* makeArrayGEP(llvm::Value* basePtr, llvm::Value* idx, const std::string& dbgName) {
    using namespace llvm;
    return builder->CreateGEP(int32Ty, basePtr, idx);
  }

  antlrcpp::Any visitProgram(LifeLangParser::ProgramContext* context) override {
    pushScope();
    registerBuiltins();
    for (auto* f : context->funcDecl()) {
      visit(f);
    }
    popScope();
    return nullptr;
  }

  antlrcpp::Any visitFuncDecl(LifeLangParser::FuncDeclContext* context) override {
    using namespace llvm;

    std::string funcName = context->ID()->getText();

    struct ParamDesc {
      std::string name;
      bool is_scalar;
      int dimX, dimY;
    };

    std::vector<ParamDesc> params;

    if (auto* pl = context->paramList()) {
      for (auto* p : pl->paramItem()) {
        ParamDesc desc{};
        desc.name = p->ID()->getText();
        if (p->LBRACK()) {
          desc.is_scalar = false;
          desc.dimX = std::stoi(p->INT_LITERAL(0)->getText());
          desc.dimY = std::stoi(p->INT_LITERAL(1)->getText());
        } else {
          desc.is_scalar = true;
          desc.dimX = desc.dimY = 0;
        }
        params.push_back(desc);
      }
    }

    std::vector<Type*> paramTypes;
    paramTypes.reserve(params.size());

    for (auto& p : params) {
      if (p.is_scalar) {
        paramTypes.push_back(int32Ty);
      } else {
        paramTypes.push_back(int32PtrTy);
      }
    }

    FunctionType* funcTy = FunctionType::get(int32Ty, paramTypes, false);
    Function* func = Function::Create(funcTy, Function::ExternalLinkage, funcName, module);

    currFunc = func;

    BasicBlock* entryBB = BasicBlock::Create(*ctx, "entry", func);
    builder->SetInsertPoint(entryBB);

    pushScope();

    unsigned idx = 0;
    for (auto& arg : func->args()) {
      ParamDesc& pd = params[idx++];
      arg.setName(pd.name);

      VarInfo varInfo{};
      varInfo.dimX = pd.dimX;
      varInfo.dimY = pd.dimY;

      if (pd.is_scalar) {
        AllocaInst* slot = builder->CreateAlloca(int32Ty, nullptr, pd.name);
        builder->CreateStore(&arg, slot);
        varInfo.val_ptr = slot;
        varInfo.is_scalar = true;
      } else {
        varInfo.val_ptr = &arg;
        varInfo.is_scalar = false;
      }

      defineVar(pd.name, varInfo);
    }

    visit(context->block());

    if (!builder->GetInsertBlock()->getTerminator()) {
      builder->CreateRet(ConstantInt::get(int32Ty, 0));
    }

    popScope();
    currFunc = nullptr;

    return nullptr;
  }

  antlrcpp::Any visitBlock(LifeLangParser::BlockContext* context) override {
    pushScope();
    for (auto* s : context->stmt()) {
      visit(s);
    }

    popScope();
    return nullptr;
  }

  antlrcpp::Any visitStmt(LifeLangParser::StmtContext* context) override {
    using namespace llvm;

    if (context->VAR()) {
      visit(context->varDeclList());
      return nullptr;
    }

    if (context->assignStmt()) {
      visit(context->assignStmt());
      return nullptr;
    }

    if (context->ifStmt()) {
      visit(context->ifStmt());
      return nullptr;
    }

    if (context->loopStmt()) {
      visit(context->loopStmt());
      return nullptr;
    }

    if (context->RETURN()) {
      Value* retVal = nullptr;
      if (context->expr()) {
        retVal = visit(context->expr()).as<Value*>();
      } else {
        retVal = ConstantInt::get(int32Ty, 0);
      }
      builder->CreateRet(retVal);
      return nullptr;
    }

    if (context->expr()) {
      visit(context->expr());
    }
    return nullptr;
  }

  antlrcpp::Any visitVarDeclList(LifeLangParser::VarDeclListContext* context) override {
    for (auto* item : context->varDeclItem()) {
      visit(item);
    }

    return nullptr;
  }

  antlrcpp::Any visitVarDeclItem(LifeLangParser::VarDeclItemContext* context) override {
    using namespace llvm;

    std::string name = context->ID()->getText();

    VarInfo varInfo{};
    varInfo.dimY = varInfo.dimX = 0;

    if (context->LBRACK()) {
      varInfo.is_scalar = false;
      varInfo.dimX = std::stoi(context->INT_LITERAL(0)->getText());
      varInfo.dimY = std::stoi(context->INT_LITERAL(1)->getText());

      int total = varInfo.dimX * varInfo.dimY;

      Value* count = ConstantInt::get(int32Ty, total);

      AllocaInst* slot = builder->CreateAlloca(int32Ty, count, name);
      varInfo.val_ptr = slot;

      defineVar(name, varInfo);

      if (context->arrayInitializer()) {
        auto* init = context->arrayInitializer();
        const auto& exprs = init->expr();

        for (size_t i = 0; i < exprs.size(); ++i) {
          Value* idxVal = ConstantInt::get(int32Ty, (int)i);
          Value* elemPtr = makeArrayGEP(varInfo.val_ptr, idxVal, name + "[]");
          Value* val = visit(exprs[i]).as<Value*>();
          builder->CreateStore(val, elemPtr);
        }
      }
    } else {
      varInfo.is_scalar = true;

      AllocaInst* slot = builder->CreateAlloca(int32Ty, nullptr, name);
      varInfo.val_ptr = slot;

      defineVar(name, varInfo);

      if (context->expr()) {
        Value* initVal = visit(context->expr()).as<Value*>();
        builder->CreateStore(initVal, slot);
      } else {
        builder->CreateStore(ConstantInt::get(int32Ty, 0), slot);
      }
    }

    return nullptr;
  }

  antlrcpp::Any visitExpr(LifeLangParser::ExprContext* context) override {
    return visit(context->logicalOrExpr());
  }

  antlrcpp::Any visitLogicalOrExpr(LifeLangParser::LogicalOrExprContext* context) override {
    using namespace llvm;

    int n = context->logicalAndExpr().size();
    if (n == 1) {
      return visit(context->logicalAndExpr(0));
    }

    Value* resBool = toBool(visit(context->logicalAndExpr(0)).as<Value*>());
    for (int i = 1; i < n; ++i) {
      Value* rhsBool = toBool(visit(context->logicalAndExpr(i)).as<Value*>());
      resBool = builder->CreateOr(resBool, rhsBool);
    }

    return (Value*)boolToInt(resBool);
  }

  antlrcpp::Any visitLogicalAndExpr(LifeLangParser::LogicalAndExprContext* context) override {
    using namespace llvm;

    int n = context->equalityExpr().size();
    if (n == 1) {
      return visit(context->equalityExpr(0));
    }

    Value* resBool = toBool(visit(context->equalityExpr(0)).as<Value*>());
    for (int i = 1; i < n; ++i) {
      Value* rhsBool = toBool(visit(context->equalityExpr(i)).as<Value*>());
      resBool = builder->CreateAnd(resBool, rhsBool);
    }

    return (Value*)boolToInt(resBool);
  }

  antlrcpp::Any visitEqualityExpr(LifeLangParser::EqualityExprContext* context) override {
    using namespace llvm;

    Value* lhs = visit(context->relationalExpr(0)).as<Value*>();

    int n = context->relationalExpr().size();
    if (n == 1) {
      return (Value*)lhs;
    }

    Value* rhs = visit(context->relationalExpr(1)).as<Value*>();
    Value* cmp = nullptr;
    if (context->EQ().size() > 0) {
      cmp = builder->CreateICmpEQ(lhs, rhs);
    } else {
      cmp = builder->CreateICmpNE(lhs, rhs);
    }

    return (Value*)boolToInt(cmp);
  }

  antlrcpp::Any visitRelationalExpr(LifeLangParser::RelationalExprContext* context) override {
    using namespace llvm;

    Value* lhs = visit(context->additiveExpr(0)).as<Value*>();

    int n = context->additiveExpr().size();
    if (n == 1) {
      return (Value*)lhs;
    }

    Value* rhs = visit(context->additiveExpr(1)).as<Value*>();
    Value* cmp = nullptr;

    if (context->LT().size() > 0) {
      cmp = builder->CreateICmpSLT(lhs, rhs);
    } else if (context->LE().size() > 0) {
      cmp = builder->CreateICmpSLE(lhs, rhs);
    } else if (context->GT().size() > 0) {
      cmp = builder->CreateICmpSGT(lhs, rhs);
    } else if (context->GE().size() > 0) {
      cmp = builder->CreateICmpSGE(lhs, rhs);
    } else {
      cmp = builder->CreateICmpEQ(lhs, rhs);
    }

    return (Value*)boolToInt(cmp);
  }

  antlrcpp::Any visitAdditiveExpr(LifeLangParser::AdditiveExprContext* context) override {
    using namespace llvm;

    Value* res = visit(context->multiplicativeExpr(0)).as<Value*>();
    int n = context->multiplicativeExpr().size();

    for (int i = 1; i < n; ++i) {
      Value* rhs = visit(context->multiplicativeExpr(i)).as<Value*>();
      auto* opToken = context->children[2 * i - 1];
      std::string op = opToken->getText();
      if (op == "+") {
        res = builder->CreateAdd(res, rhs);
      } else {
        res = builder->CreateSub(res, rhs);
      }
    }

    return (Value*)res;
  }

  antlrcpp::Any visitMultiplicativeExpr(LifeLangParser::MultiplicativeExprContext* context) override {
    using namespace llvm;

    Value* res = visit(context->unaryExpr(0)).as<Value*>();
    int n = context->unaryExpr().size();

    for (int i = 1; i < n; ++i) {
      Value* rhs = visit(context->unaryExpr(i)).as<Value*>();
      auto* opToken = context->children[2 * i - 1];
      std::string op = opToken->getText();
      if (op == "*") {
        res = builder->CreateMul(res, rhs);
      } else if (op == "/") {
        res = builder->CreateSDiv(res, rhs);
      } else {
        res = builder->CreateSRem(res, rhs);
      }
    }

    return (Value*)res;
  }

  antlrcpp::Any visitUnaryExpr(LifeLangParser::UnaryExprContext* context) override {
    using namespace llvm;

    if (context->MINUS()) {
      Value* v = visit(context->unaryExpr()).as<Value*>();
      return (Value*)builder->CreateNeg(v);
    }

    if (context->NOT()) {
      Value* v = visit(context->unaryExpr()).as<Value*>();
      Value* b = toBool(v);
      Value* nb = builder->CreateNot(b);
      return (Value*)boolToInt(nb);
    }

    if (context->INC_OP()) {
      std::string name = context->ID()->getText();
      VarInfo* info = lookUpVar(name);
      Value* oldVal = builder->CreateLoad(int32Ty, info->val_ptr);
      Value* newVal = builder->CreateAdd(oldVal, ConstantInt::get(int32Ty, 1));
      builder->CreateStore(newVal, info->val_ptr);

      return (Value*)newVal;
    }

    return visit(context->primaryExpr());
  }

  antlrcpp::Any visitPrimaryExpr(LifeLangParser::PrimaryExprContext* context) override {
    using namespace llvm;

    if (context->INT_LITERAL()) {
      int v = std::stoi(context->INT_LITERAL()->getText());
      return (Value*)ConstantInt::get(int32Ty, v);
    }

    if (context->ID() && context->LPAREN() && !context->LBRACK()) {
      std::string fname = context->ID()->getText();
      Function* callee = module->getFunction(fname);

      std::vector<Value*> args;
      if (context->argList()) {
        auto argValsAny = visit(context->argList()).as<std::vector<Value*>>();
        args = std::move(argValsAny);
      }

      CallInst* call = builder->CreateCall(callee, args);
      if (callee->getReturnType()->isVoidTy()) {
        return (Value*)ConstantInt::get(int32Ty, 0);
      }
      return (Value*)call;
    }

    if (context->ID() && context->LBRACK()) {
      std::string name = context->ID()->getText();
      VarInfo* info = lookUpVar(name);
      Value* ix = visit(context->expr(0)).as<Value*>();
      Value* iy = visit(context->expr(1)).as<Value*>();

      Value* dimYVal = ConstantInt::get(int32Ty, info->dimY);
      Value* mul     = builder->CreateMul(ix, dimYVal);
      Value* idx     = builder->CreateAdd(mul, iy);

      Value* elemPtr = makeArrayGEP(info->val_ptr, idx, name + "[]");
      Value* val     = builder->CreateLoad(int32Ty, elemPtr);
      return (Value*)val;
    }

    if (context->LPAREN() && context->RPAREN()) {
      return visit(context->expr(0));
    }

    if (context->ID()) {
      std::string name = context->ID()->getText();
      VarInfo* info = lookUpVar(name);
      if (info->is_scalar) {
        Value* val = builder->CreateLoad(int32Ty, info->val_ptr);
        return (Value*)val;
      } else {
        return (Value*)info->val_ptr;
      }
    }

    return (Value*)ConstantInt::get(int32Ty, 0);
  }

  antlrcpp::Any visitArgList(LifeLangParser::ArgListContext* context) override {
    using namespace llvm;

    std::vector<Value*> args;
    args.reserve(context->expr().size());
    for (auto* e : context->expr()) {
      args.push_back(visit(e).as<Value*>());
    }
    return args;
  }

  antlrcpp::Any visitAssignStmt(LifeLangParser::AssignStmtContext* context) override {
    using namespace llvm;

    std::string name = context->ID()->getText();
    VarInfo* info = lookUpVar(name);

    if (context->LBRACK()) {
      Value* ix  = visit(context->expr(0)).as<Value*>();
      Value* iy  = visit(context->expr(1)).as<Value*>();
      Value* rhs = visit(context->expr(2)).as<Value*>();

      Value* dimYVal = ConstantInt::get(int32Ty, info->dimY);
      Value* mul     = builder->CreateMul(ix, dimYVal);
      Value* idx     = builder->CreateAdd(mul, iy);

      Value* elemPtr = makeArrayGEP(info->val_ptr, idx, name + "[]");
      builder->CreateStore(rhs, elemPtr);
      return nullptr;
    }

    if (info->is_scalar) {
      Value* rhs = visit(context->expr(0)).as<Value*>();
      builder->CreateStore(rhs, info->val_ptr);
      return nullptr;
    }

    Value* rhsVal = visit(context->expr(0)).as<Value*>();
    Type* rhsTy   = rhsVal->getType();

    int total = info->dimX * info->dimY;
    Value* totalVal = ConstantInt::get(int32Ty, total);

    AllocaInst* iAlloca = builder->CreateAlloca(int32Ty, nullptr, name + "_i");
    builder->CreateStore(ConstantInt::get(int32Ty, 0), iAlloca);

    Function* f = currFunc;
    BasicBlock* condBB = BasicBlock::Create(*ctx, "arrassign.cond", f);
    BasicBlock* bodyBB = BasicBlock::Create(*ctx, "arrassign.body", f);
    BasicBlock* exitBB = BasicBlock::Create(*ctx, "arrassign.exit", f);

    builder->CreateBr(condBB);

    builder->SetInsertPoint(condBB);
    Value* iVal = builder->CreateLoad(int32Ty, iAlloca);
    Value* cond = builder->CreateICmpSLT(iVal, totalVal);
    builder->CreateCondBr(cond, bodyBB, exitBB);

    builder->SetInsertPoint(bodyBB);

    if (rhsTy->isPointerTy()) {
      Value* destElemPtr = makeArrayGEP(info->val_ptr, iVal, name + ".dst[]");
      Value* srcElemPtr  = makeArrayGEP(rhsVal,        iVal, name + ".src[]");
      Value* srcVal      = builder->CreateLoad(int32Ty, srcElemPtr);
      builder->CreateStore(srcVal, destElemPtr);
    } else {
      Value* scalar = rhsVal;
      if (scalar->getType() != int32Ty && scalar->getType()->isIntegerTy()) {
        scalar = builder->CreateIntCast(scalar, int32Ty, /*isSigned*/true);
      }
      Value* destElemPtr = makeArrayGEP(info->val_ptr, iVal, name + ".fill[]");
      builder->CreateStore(scalar, destElemPtr);
    }

    Value* iNext = builder->CreateAdd(iVal, ConstantInt::get(int32Ty, 1));
    builder->CreateStore(iNext, iAlloca);
    builder->CreateBr(condBB);

    builder->SetInsertPoint(exitBB);

    return nullptr;
  }

  antlrcpp::Any visitIfStmt(LifeLangParser::IfStmtContext* context) override {
    using namespace llvm;

    Value* condVal = visit(context->expr()).as<Value*>();
    Value* condBool = toBool(condVal);

    Function* f = currFunc;
    BasicBlock* thenBB = BasicBlock::Create(*ctx, "if.then", f);
    BasicBlock* elseBB = nullptr;
    BasicBlock* mergeBB = BasicBlock::Create(*ctx, "if.end", f);

    if (context->ELSE()) {
      elseBB = BasicBlock::Create(*ctx, "if.else", f);
      builder->CreateCondBr(condBool, thenBB, elseBB);
    } else {
      builder->CreateCondBr(condBool, thenBB, mergeBB);
    }

    builder->SetInsertPoint(thenBB);
    visit(context->block(0));
    if (!builder->GetInsertBlock()->getTerminator()) {
      builder->CreateBr(mergeBB);
    }

    if (context->ELSE()) {
      builder->SetInsertPoint(elseBB);
      visit(context->block(1));
      if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
      }
    }

    builder->SetInsertPoint(mergeBB);
    return nullptr;
  }

  antlrcpp::Any visitLoopStmt(LifeLangParser::LoopStmtContext* context) override {
    using namespace llvm;

    Function* f = currFunc;
    if (!context->LPAREN()) {
      BasicBlock* loopBB = BasicBlock::Create(*ctx, "loop.body", f);
      BasicBlock* afterBB = BasicBlock::Create(*ctx, "loop.after", f);

      builder->CreateBr(loopBB);

      builder->SetInsertPoint(loopBB);
      visit(context->block());
      if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(loopBB);
      }

      builder->SetInsertPoint(afterBB);
      return nullptr;
    }

    Value* condExprVal = nullptr;

    BasicBlock* condBB = BasicBlock::Create(*ctx, "loop.cond", f);
    BasicBlock* bodyBB = BasicBlock::Create(*ctx, "loop.body", f);
    BasicBlock* afterBB = BasicBlock::Create(*ctx, "loop.after", f);

    builder->CreateBr(condBB);

    builder->SetInsertPoint(condBB);
    condExprVal = visit(context->expr(0)).as<Value*>();
    Value* condBool = toBool(condExprVal);
    builder->CreateCondBr(condBool, bodyBB, afterBB);

    builder->SetInsertPoint(bodyBB);
    visit(context->block());
    if (!builder->GetInsertBlock()->getTerminator()) {
      visit(context->expr(1)).as<Value*>();
      if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(condBB);
      }
    }

    builder->SetInsertPoint(afterBB);
    return nullptr;
  }
};

int main(int argc, char *argv[]) {
  using namespace llvm;
  if (argc != 2) {
    outs() << "[ERROR]: Need argument: file with LifeLang\n";
    return 1;
  }

  std::ifstream stream;
  stream.open(argv[1]);

  antlr4::ANTLRInputStream input(stream);

  LifeLangLexer lexer(&input);

  antlr4::CommonTokenStream tokens(&lexer);

  LifeLangParser parser(&tokens);

  LLVMContext context;
  llvm::Module *module = new Module("top", context);
  IRBuilder<> builder(context);

  TreeLLVMWalker walker(&context, &builder, module);
  walker.visitProgram(parser.program());

  outs() << "[LLVM IR]\n";
  module->print(outs(), nullptr);
  outs() << '\n';

  bool broken = verifyModule(*module, &outs());
  if (broken) {
    outs() << "[VERIFICATION] FAIL\n";
    return 1;
  }
  outs() << "[VERIFICATION] OK\n\n";

  Function *mainFunc = module->getFunction("main");
  if (mainFunc == nullptr) {
    outs() << "Can't find main function\n";
    return -1;
  }

  outs() << "[EE] Run\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
  ee->InstallLazyFunctionCreator([](const std::string &fnName) -> void * {
    if (fnName == "put_pixel") {
      return reinterpret_cast<void *>(simPutPixel);
    }
    if (fnName == "flush") {
      return reinterpret_cast<void *>(simFlush);
    }
    if (fnName == "rand") {
      return reinterpret_cast<void*>(simRand);
    }
    outs() << "[ExecutionEngine] Can't find function " << fnName
           << ". Catch the Segmentation fault:)\n";
    return nullptr;
  });
  ee->finalizeObject();

  if (std::string(argv[1]) == "app.llang") {
    simInit();
  }

  ArrayRef<GenericValue> noargs;
  GenericValue v = ee->runFunction(mainFunc, noargs);
  outs() << "[EE] Result: " << v.IntVal << "\n";

  if (std::string(argv[1]) == "app.llang") {
    simExit();
  }

  return 0;
}