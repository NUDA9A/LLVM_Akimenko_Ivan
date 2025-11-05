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

  ArrayType *regFileType = ArrayType::get(builder.getInt64Ty(), REG_FILE_SIZE);
  GlobalVariable *regFile = new GlobalVariable(
      *module, regFileType, false, GlobalValue::InternalLinkage, ConstantAggregateZero::get(regFileType), "regFile");

  Type *int32Type = Type::getInt32Ty(context);
  Type *int64Type = Type::getInt64Ty(context);
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
  FunctionType *int32x3FuncType =
      FunctionType::get(voidType, int32x3Types, false);
  FunctionType* randType = FunctionType::get(int32Type, false);
  // Functions
  FunctionCallee simPutPixelFunc =
      module->getOrInsertFunction("simPutPixel", int32x3FuncType);
  FunctionCallee simRandFunc =
      module->getOrInsertFunction("simRand", randType);
  FunctionCallee simFlushFunc =
      module->getOrInsertFunction("simFlush", voidFuncType);

  while (input >> name) {
    if (!name.compare("D_EQi_AND")) {
      Value* x5_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 4);
      Value* x6_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 5);
      Value* x7_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 6);
      input >> arg;
      outs() << "\tD_EQi_AND " << arg;
      Value* arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg;
      Value *arg2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " " << arg;
      Value* arg3 = builder.getInt64(std::stol(arg));
      input >> arg;
      outs() << " " << arg << "\n";
      Value* arg4 = builder.getInt64(std::stol(arg));
      Value* eq1 = builder.CreateICmpEQ(builder.CreateLoad(int64Type, arg1_ptr), arg3);
      Value* eq2 = builder.CreateICmpEQ(builder.CreateLoad(int64Type, arg2_ptr), arg4);
      Value* eq1_i64 = builder.CreateZExt(eq1, int64Type);
      Value* eq2_i64 = builder.CreateZExt(eq2, int64Type);
      builder.CreateStore(eq1_i64, x5_ptr);
      builder.CreateStore(eq2_i64, x6_ptr);
      Value* eq1_and_eq2 = builder.CreateAnd(eq1, eq2);
      Value* eq1_and_eq2_i64 = builder.CreateZExt(eq1_and_eq2, int64Type);
      builder.CreateStore(eq1_and_eq2_i64, x7_ptr);
      continue;
    }
    if (!name.compare("PUT_PIXEL")) {
      input >> arg;
      outs() << "\tPUT_PIXEL " << arg;
      Value *arg1x64 = builder.CreateLoad(int64Type, builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1));
      Value *arg1x32 = builder.CreateTrunc(arg1x64, int32Type);
      input >> arg;
      outs() << " " << arg;
      Value *arg2x64 = builder.CreateLoad(int64Type, builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1));
      Value *arg2x32 = builder.CreateTrunc(arg2x64, int32Type);
      input >> arg;
      outs() << " " << arg << '\n';
      Value *arg3 = builder.getInt32(std::stoi(arg.substr(2), nullptr, 16));
      Value *args[] = {arg1x32, arg2x32, arg3};
      builder.CreateCall(simPutPixelFunc, args);
      continue;
    }
    if (!name.compare("RAND")) {
      input >> arg;
      outs() << "\t" << arg << " = RAND\n";
      Value *res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* res_i32 = builder.CreateCall(simRandFunc);
      Value* res_i64 = builder.CreateZExt(res_i32, int64Type);
      builder.CreateStore(res_i64, res_ptr);
      continue;
    }
    if (!name.compare("FLUSH")) {
      outs() << "\tFLUSH\n";
      builder.CreateCall(simFlushFunc);
      continue;
    }
    if (!name.compare("XOR")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " ^ " << arg << '\n';
      // arg2
      Value *arg2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* xor_arg1_arg2 = builder.CreateXor(builder.CreateLoad(int64Type, arg1_ptr), builder.CreateLoad(int64Type, arg2_ptr));
      builder.CreateStore(xor_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("AND")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " & " << arg << '\n';
      // arg2
      Value *arg2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* and_arg1_arg2 = builder.CreateAnd(builder.CreateLoad(int64Type, arg1_ptr), builder.CreateLoad(int64Type, arg2_ptr));
      builder.CreateStore(and_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("ADD")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " + " << arg << '\n';
      // arg2
      Value *arg2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* add_arg1_arg2 = builder.CreateAdd(builder.CreateLoad(int64Type, arg1_ptr), builder.CreateLoad(int64Type, arg2_ptr));
      builder.CreateStore(add_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("OR")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " | " << arg << '\n';
      // arg2
      Value *arg2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* or_arg1_arg2 = builder.CreateOr(builder.CreateLoad(int64Type, arg1_ptr), builder.CreateLoad(int64Type, arg2_ptr));
      builder.CreateStore(or_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("SUBi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " - " << arg << '\n';
      // arg2
      Value *arg2 = builder.getInt64(std::stol(arg));
      Value* subi_arg1_arg2 = builder.CreateSub(builder.CreateLoad(int64Type, arg1_ptr), arg2);
      builder.CreateStore(subi_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("INC_NEi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value *res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0,
                                                std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = ++" << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0,
                                                 std::stoi(arg.substr(1)) - 1);
      Value *arg1 = builder.CreateAdd(builder.CreateLoad(int64Type, arg1_ptr),
                                      builder.getInt64(1));
      builder.CreateStore(arg1, arg1_ptr);
      input >> arg;
      outs() << " != " << arg << '\n';
      // arg2
      Value *arg2 = builder.getInt64(std::stol(arg));
      Value *cond = builder.CreateICmpNE(arg1, arg2);
      Value* cond_i64 = builder.CreateZExt(cond, int64Type);
      builder.CreateStore(cond_i64, res_ptr);
      continue;
    }
    if (!name.compare("ANDi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " & " << arg << '\n';
      // arg2
      Value *arg2 = builder.getInt64(std::stol(arg));
      Value* andi_arg1_arg2 = builder.CreateAnd(builder.CreateLoad(int64Type, arg1_ptr), arg2);
      builder.CreateStore(andi_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("ADDi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " + " << arg << '\n';
      // arg2
      Value *arg2 = builder.getInt64(std::stol(arg));
      Value* addi_arg1_arg2 = builder.CreateAdd(builder.CreateLoad(int64Type, arg1_ptr), arg2);
      builder.CreateStore(addi_arg1_arg2, res_ptr);
      continue;
    }
    if (!name.compare("EQi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " == " << arg << '\n';
      // arg2
      Value *arg2 = builder.getInt64(std::stol(arg));
      Value* eqi_arg1_arg2 = builder.CreateICmpEQ(builder.CreateLoad(int64Type, arg1_ptr), arg2);
      Value* eqi_arg1_arg2_i64 = builder.CreateZExt(eqi_arg1_arg2, int64Type);
      builder.CreateStore(eqi_arg1_arg2_i64, res_ptr);
      continue;
    }
    if (!name.compare("LOAD_FIELD")) {
      Value* x1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 0);
      Value* x2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 1);
      Value* x = builder.CreateLoad(int64Type, x1_ptr);
      Value* y = builder.CreateLoad(int64Type, x2_ptr);
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg << "[x1][x2]";
      // arg1
      Value *base_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* base_i64 = builder.CreateLoad(int64Type, base_ptr);
      Value* base_i32_ptr = builder.CreateIntToPtr(base_i64, int32Type->getPointerTo());

      Value* y_size = builder.getInt64(SIM_Y_SIZE);
      Value* x_mul_y_size = builder.CreateMul(x, y_size);
      Value* idx = builder.CreateAdd(x_mul_y_size, y);

      Value* elem_ptr = builder.CreateInBoundsGEP(int32Type, base_i32_ptr, idx);
      Value* val_i32 = builder.CreateLoad(int32Type, elem_ptr);
      Value* val_i64 = builder.CreateZExt(val_i32, int64Type);
      builder.CreateStore(val_i64, res_ptr);
      continue;
    }
    if (!name.compare("MOV")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      // arg1
      Value *arg1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      // arg2
      Value* arg1_val = builder.CreateLoad(int64Type, arg1_ptr);
      builder.CreateStore(arg1_val, res_ptr);
      continue;
    }
    if (!name.compare("MOVi")) {
      input >> arg;
      outs() << "\t" << arg;
      // res
      Value* res_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      input >> arg;
      outs() << " = " << arg;
      Value *arg1 = builder.getInt64(std::stol(arg));
      builder.CreateStore(arg1, res_ptr);
      continue;
    }
    if (!name.compare("STORE_FIELD")) {
      Value* x1_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 0);
      Value* x2_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 1);
      Value* x6_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, 5);
      Value* x = builder.CreateLoad(int64Type, x1_ptr);
      Value* y = builder.CreateLoad(int64Type, x2_ptr);
      Value* x6_i64 = builder.CreateLoad(int64Type, x6_ptr);
      Value* x6_i32 = builder.CreateTrunc(x6_i64, int32Type);
      input >> arg;
      outs() << "\t" << arg << "[x1][x2] = x6\n";

      Value *base_ptr = builder.CreateConstGEP2_64(regFileType, regFile, 0, std::stoi(arg.substr(1)) - 1);
      Value* base_i64 = builder.CreateLoad(int64Type, base_ptr);
      Value* base_i32_ptr = builder.CreateIntToPtr(base_i64, int32Type->getPointerTo());

      Value* y_size = builder.getInt64(SIM_Y_SIZE);
      Value* x_mul_y_size = builder.CreateMul(x, y_size);
      Value* idx = builder.CreateAdd(x_mul_y_size, y);

      Value* elem_ptr = builder.CreateInBoundsGEP(int32Type, base_i32_ptr, idx);

      builder.CreateStore(x6_i32, elem_ptr);
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
    if (fnName == "simPutPixel") {
      return reinterpret_cast<void *>(simPutPixel);
    }
    if (fnName == "simRand") {
      return reinterpret_cast<void *>(simRand);
    }
    if (fnName == "simFlush") {
      return reinterpret_cast<void *>(simFlush);
    }
    return nullptr;
  });
  ee->finalizeObject();

  simInit();

  ArrayRef<GenericValue> noargs;
  ee->runFunction(mainFunc, noargs);
  outs() << "#[Code was run]\n";

  simExit();
  return EXIT_SUCCESS;
}