#include "../SDL/sim.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
using namespace llvm;

const int REG_FILE_SIZE = 13;
size_t REG_FILE[REG_FILE_SIZE];

void do_XOR(int arg1, int arg2, int arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] ^ REG_FILE[arg3];
}

void do_SUBi(int arg1, int arg2, size_t arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] - arg3;
}

void do_PUT_PIXEL(int arg1, int arg2, size_t arg3) {
  simPutPixel(REG_FILE[arg1], REG_FILE[arg2], arg3);
}

void do_INC_NEi(int arg1, int arg2, size_t arg3) {
  REG_FILE[arg2]++;
  REG_FILE[arg1] = REG_FILE[arg2] != arg3;
}

void do_FLUSH() { simFlush(); }

void do_RAND(int arg1) { REG_FILE[arg1] = simRand(); }

void do_ANDi(int arg1, int arg2, size_t arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] & arg3;
}

void do_AND(int arg1, int arg2, int arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] & REG_FILE[arg3];
}

void do_STORE_FIELD(int arg1) {
  int *base = reinterpret_cast<int *>(REG_FILE[arg1]);
  int indx = REG_FILE[0] * SIM_Y_SIZE + REG_FILE[1];
  base[indx] = REG_FILE[5];
}

void do_LOAD_FIELD(int arg1, int arg2) {
  int *base = reinterpret_cast<int *>(REG_FILE[arg2]);
  size_t indx = REG_FILE[0] * SIM_Y_SIZE + REG_FILE[1];
  REG_FILE[arg1] = base[indx];
}

void do_MOV(int arg1, int arg2) { REG_FILE[arg1] = REG_FILE[arg2]; }

void do_MOVi(int arg1, size_t arg2) { REG_FILE[arg1] = arg2; }

void do_D_EQi_AND(int arg1, int arg2, size_t arg3, size_t arg4) {
  REG_FILE[4] = REG_FILE[arg1] == arg3;
  REG_FILE[5] = REG_FILE[arg2] == arg4;
  REG_FILE[6] = REG_FILE[4] & REG_FILE[5];
}

void do_ADD(int arg1, int arg2, int arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] + REG_FILE[arg3];
}

void do_ADDi(int arg1, int arg2, size_t arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] + arg3;
}

void do_EQi(int arg1, int arg2, size_t arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] == arg3;
}

void do_OR(int arg1, int arg2, int arg3) {
  REG_FILE[arg1] = REG_FILE[arg2] | REG_FILE[arg3];
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    outs() << "[ERROR] Need 1 argument: file with assembler code\n";
    return 1;
  }
  std::ifstream input;
  input.open(argv[1]);
  if (!input.is_open()) {
    outs() << "[ERROR] Can't open " << argv[1] << '\n';
    return 1;
  }

  LLVMContext context;
  // ; ModuleID = 'top'
  // source_filename = "top"
  Module *module = new Module("top", context);
  IRBuilder<> builder(context);

  //[32 x i32] regFile = {0, 0, 0, 0}
  ArrayType *regFileType = ArrayType::get(builder.getInt64Ty(), REG_FILE_SIZE);
  module->getOrInsertGlobal("regFile", regFileType);
  GlobalVariable *regFile = module->getNamedGlobal("regFile");

  Type *int32Type = Type::getInt32Ty(context);
  ArrayType *int32ArrayType = ArrayType::get(int32Type, 500);
  ArrayType *int32ArrayArrayType = ArrayType::get(int32ArrayType, 500);

  // declare void @main()
  FunctionType *funcType = FunctionType::get(builder.getVoidTy(), false);
  Function *mainFunc =
      Function::Create(funcType, Function::ExternalLinkage, "main", module);

  BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entryBB);

  std::string name;
  std::string arg;
  std::unordered_map<std::string, BasicBlock *> BBMap;

  AllocaInst* fieldAlloca = builder.CreateAlloca(int32ArrayArrayType);
  fieldAlloca->setAlignment(Align(16));
  AllocaInst* newFieldAlloca = builder.CreateAlloca(int32ArrayArrayType);
  newFieldAlloca->setAlignment(Align(16));

  Value* fieldPtrInt = builder.CreatePtrToInt(fieldAlloca, builder.getInt64Ty());
  Value* newFieldPtrInt = builder.CreatePtrToInt(newFieldAlloca, builder.getInt64Ty());
  Value* fieldPtr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 9);
  Value* newFieldPtr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 10);
  builder.CreateStore(fieldPtrInt, fieldPtr);
  builder.CreateStore(newFieldPtrInt, newFieldPtr);

  outs() << "\n#[FILE]:\nBBs:";

  while (input >> name) {
    if (!name.compare("D_EQi_AND")) {
      input >> arg >> arg >> arg >> arg;
      continue;
    }
    if (!name.compare("XOR") || !name.compare("OR") || !name.compare("SUBi") ||
        !name.compare("PUT_PIXEL") || !name.compare("INC_NEi") ||
        !name.compare("EQi") || !name.compare("ANDi") || !name.compare("AND") ||
        !name.compare("ADD") || !name.compare("ADDi")) {
      input >> arg >> arg >> arg;
      continue;
    }
    if (!name.compare("BR_COND") || !name.compare("LOAD_FIELD") ||
        !name.compare("MOV") || !name.compare("MOVi")) {
      input >> arg >> arg;
      continue;
    }
    if (!name.compare("RAND") || !name.compare("STORE_FIELD")) {
      input >> arg;
      continue;
    }
    if (!name.compare("FLUSH")) {
      continue;
    }

    outs() << " " << name;
    BBMap[name] = BasicBlock::Create(context, name, mainFunc);
  }
  outs() << '\n';
  input.close();
  input.open(argv[1]);

  // Functions types
  Type *voidType = Type::getVoidTy(context);
  FunctionType *voidFuncType = FunctionType::get(voidType, false);
  ArrayRef<Type *> int32x3Types = {Type::getInt32Ty(context),
                                   Type::getInt32Ty(context),
                                   Type::getInt32Ty(context)};
  ArrayRef<Type *> int32x2sizetTypes = {Type::getInt32Ty(context),
                                        Type::getInt32Ty(context),
                                        Type::getInt64Ty(context)};
  ArrayRef<Type *> int32x1Types = {Type::getInt32Ty(context)};
  ArrayRef<Type *> int32x2Types = {Type::getInt32Ty(context),
                                   Type::getInt32Ty(context)};
  ArrayRef<Type *> int32x2sizetx2Types = {
      Type::getInt32Ty(context), Type::getInt32Ty(context),
      Type::getInt64Ty(context), Type::getInt64Ty(context)};
  ArrayRef<Type *> int32sizetTypes = {Type::getInt32Ty(context),
                                      Type::getInt64Ty(context)};

  FunctionType *int32sizetFuncType =
      FunctionType::get(voidType, int32sizetTypes, false);
  FunctionType *int32x2sizetx2FuncType =
      FunctionType::get(voidType, int32x2sizetx2Types, false);
  FunctionType *int32x2FuncType =
      FunctionType::get(voidType, int32x2Types, false);
  FunctionType *int32x1FuncType =
      FunctionType::get(voidType, int32x1Types, false);
  FunctionType *int32x2sizetFuncType =
      FunctionType::get(voidType, int32x2sizetTypes, false);
  FunctionType *int32x3FuncType =
      FunctionType::get(voidType, int32x3Types, false);
  // Functions
  // int32x2sizex2FuncType
  FunctionCallee do_D_EQi_ANDFunc =
      module->getOrInsertFunction("do_D_EQi_AND", int32x2sizetx2FuncType);

  // int32x3FuncType
  FunctionCallee do_XORFunc =
      module->getOrInsertFunction("do_XOR", int32x3FuncType);

  FunctionCallee do_ANDFunc =
      module->getOrInsertFunction("do_AND", int32x3FuncType);

  FunctionCallee do_ADDFunc =
      module->getOrInsertFunction("do_ADD", int32x3FuncType);

  FunctionCallee do_ORFunc =
      module->getOrInsertFunction("do_OR", int32x3FuncType);

  // int32x2sizetFuncType
  FunctionCallee do_PUT_PIXELFunc =
      module->getOrInsertFunction("do_PUT_PIXEL", int32x2sizetFuncType);

  FunctionCallee do_SUBiFunc =
      module->getOrInsertFunction("do_SUBi", int32x2sizetFuncType);

  FunctionCallee do_INC_NEiFunc =
      module->getOrInsertFunction("do_INC_NEi", int32x2sizetFuncType);

  FunctionCallee do_ANDiFunc =
      module->getOrInsertFunction("do_ANDi", int32x2sizetFuncType);

  FunctionCallee do_ADDiFunc =
      module->getOrInsertFunction("do_ADDi", int32x2sizetFuncType);

  FunctionCallee do_EQiFunc =
      module->getOrInsertFunction("do_EQi", int32x2sizetFuncType);

  // int32x2FuncType
  FunctionCallee do_LOAD_FIELDFunc =
      module->getOrInsertFunction("do_LOAD_FIELD", int32x2FuncType);

  FunctionCallee do_MOVFunc =
      module->getOrInsertFunction("do_MOV", int32x2FuncType);

  // int32sizetFuncType
  FunctionCallee do_MOViFunc =
      module->getOrInsertFunction("do_MOVi", int32sizetFuncType);

  // int32x1FuncType
  FunctionCallee do_RANDFunc =
      module->getOrInsertFunction("do_RAND", int32x1FuncType);

  FunctionCallee do_STORE_FIELDFunc =
      module->getOrInsertFunction("do_STORE_FIELD", int32x1FuncType);

  FunctionCallee do_FLUSHFunc =
      module->getOrInsertFunction("do_FLUSH", voidFuncType);

  while (input >> name) {
    if (!name.compare("D_EQi_AND")) {
      input >> arg;
      outs() << "\tD_EQi_AND " << arg;
      Value* arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg;
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg;
      Value* arg3 = builder.getInt64(std::stol(arg));
      input >> arg;
      outs() << " " << arg << "\n";
      Value* arg4 = builder.getInt64(std::stol(arg));
      Value* args[] = {arg1, arg2, arg3, arg4};
      builder.CreateCall(do_D_EQi_ANDFunc, args);
      continue;
    }
    if (!name.compare("PUT_PIXEL")) {
      input >> arg;
      outs() << "\tPUT_PIXEL " << arg;
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg;
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg << '\n';
      Value *arg3 = builder.getInt64(std::stol(arg.substr(2), nullptr, 16));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_PUT_PIXELFunc, args);
      continue;
    }
    if (!name.compare("RAND")) {
      input >> arg;
      outs() << '\t' << arg << " = RAND\n";
      Value* arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      builder.CreateCall(do_RANDFunc, {arg1});
      continue;
    }
    if (!name.compare("FLUSH")) {
      outs() << "\tFLUSH\n";
      builder.CreateCall(do_FLUSHFunc);
      continue;
    }
    if (!name.compare("XOR")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " ^ " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_XORFunc, args);
      continue;
    }
    if (!name.compare("AND")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " & " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_ANDFunc, args);
      continue;
    }
    if (!name.compare("ADD")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " + " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_ADDFunc, args);
      continue;
    }
    if (!name.compare("OR")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " | " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_ORFunc, args);
      continue;
    }
    if (!name.compare("SUBi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " - " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt64(std::stol(arg));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_SUBiFunc, args);
      continue;
    }
    if (!name.compare("INC_NEi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = ++" << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " != " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt64(std::stol(arg));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_INC_NEiFunc, args);
      continue;
    }
    if (!name.compare("ANDi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " & " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt64(std::stol(arg));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_ANDiFunc, args);
      continue;
    }
    if (!name.compare("ADDi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " + " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt64(std::stol(arg));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_ADDiFunc, args);
      continue;
    }
    if (!name.compare("EQi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " == " << arg << '\n';
      // arg2
      Value *arg3 = builder.getInt64(std::stol(arg));
      Value *args[] = {arg1, arg2, arg3};
      builder.CreateCall(do_EQiFunc, args);
      continue;
    }
    if (!name.compare("LOAD_FIELD")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg << "[x1][x2]\n";
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      // arg2
      Value *args[] = {arg1, arg2};
      builder.CreateCall(do_LOAD_FIELDFunc, args);
      continue;
    }
    if (!name.compare("MOV")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      // arg2
      Value *args[] = {arg1, arg2};
      builder.CreateCall(do_MOVFunc, args);
      continue;
    }
    if (!name.compare("MOVi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg2 = builder.getInt64(std::stol(arg));
      // arg2
      Value *args[] = {arg1, arg2};
      builder.CreateCall(do_MOViFunc, args);
      continue;
    }
    if (!name.compare("STORE_FIELD")) {
      input >> arg;
      outs() << '\t' << arg << "[x1][x2] = " << "x6\n";
      Value* arg1 = builder.getInt32(std::stoi(arg.substr(1)) - 1);
      builder.CreateCall(do_STORE_FIELDFunc, {arg1});
      continue;
    }
    if (!name.compare("BR_COND")) {
      input >> arg;
      outs() << "\tif (" << arg;
      // reg1
      Value *reg_p = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value *reg_i1 = builder.CreateTrunc(
          builder.CreateLoad(builder.getInt64Ty(), reg_p), builder.getInt1Ty());
      input >> arg;
      outs() << ") then BB:" << arg;
      input >> name;
      outs() << " else BB:" << name << '\n';
      outs() << "BB " << name << '\n';
      builder.CreateCondBr(reg_i1, BBMap[arg], BBMap[name]);
      builder.SetInsertPoint(BBMap[name]);
      continue;
    }
    if (!name.compare("BR")) {
      input >> arg;
      outs() << "\t branch to : " << arg << '\n';
      builder.CreateBr(BBMap[arg]);
      continue;
    }

    if (BasicBlock *cur = builder.GetInsertBlock()) {
      if (!cur->getTerminator()) {
        builder.CreateBr(BBMap[name]);
        outs() << "\tbranch to " << name << '\n';
      }
    }
    outs() << "BB " << name << '\n';
    builder.SetInsertPoint(BBMap[name]);
  }

  outs() << "\n#[LLVM IR]:\n";
  module->print(outs(), nullptr);
  outs() << '\n';
  bool verif = verifyFunction(*mainFunc, &outs());
  outs() << "[VERIFICATION] " << (verif ? "FAIL\n\n" : "OK\n\n");

  outs() << "\n#[Running code]\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  ExecutionEngine *ee = EngineBuilder(std::unique_ptr<Module>(module)).create();
  ee->InstallLazyFunctionCreator([](const std::string &fnName) -> void * {
    if (fnName == "do_D_EQi_AND") {
      return reinterpret_cast<void *>(do_D_EQi_AND);
    }
    if (fnName == "do_XOR") {
      return reinterpret_cast<void *>(do_XOR);
    }
    if (fnName == "do_PUT_PIXEL") {
      return reinterpret_cast<void *>(do_PUT_PIXEL);
    }
    if (fnName == "do_AND") {
      return reinterpret_cast<void *>(do_AND);
    }
    if (fnName == "do_ADD") {
      return reinterpret_cast<void *>(do_ADD);
    }
    if (fnName == "do_OR") {
      return reinterpret_cast<void *>(do_OR);
    }
    if (fnName == "do_SUBi") {
      return reinterpret_cast<void *>(do_SUBi);
    }
    if (fnName == "do_INC_NEi") {
      return reinterpret_cast<void *>(do_INC_NEi);
    }
    if (fnName == "do_ANDi") {
      return reinterpret_cast<void *>(do_ANDi);
    }
    if (fnName == "do_ADDi") {
      return reinterpret_cast<void *>(do_ADDi);
    }
    if (fnName == "do_EQi") {
      return reinterpret_cast<void *>(do_EQi);
    }
    if (fnName == "do_LOAD_FIELD") {
      return reinterpret_cast<void *>(do_LOAD_FIELD);
    }
    if (fnName == "do_MOV") {
      return reinterpret_cast<void *>(do_MOV);
    }
    if (fnName == "do_MOVi") {
      return reinterpret_cast<void *>(do_MOVi);
    }
    if (fnName == "do_RAND") {
      return reinterpret_cast<void *>(do_RAND);
    }
    if (fnName == "do_STORE_FIELD") {
      return reinterpret_cast<void *>(do_STORE_FIELD);
    }
    if (fnName == "do_FLUSH") {
      return reinterpret_cast<void *>(do_FLUSH);
    }
    return nullptr;
  });

  ee->addGlobalMapping(regFile, (void *)REG_FILE);
  ee->finalizeObject();

  simInit();

  ArrayRef<GenericValue> noargs;
  ee->runFunction(mainFunc, noargs);
  outs() << "#[Code was run]\n";

  simExit();
  return EXIT_SUCCESS;
}