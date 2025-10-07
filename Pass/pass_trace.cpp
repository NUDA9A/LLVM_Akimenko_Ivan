#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

struct MyPass : public PassInfoMixin<MyPass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
    LLVMContext& Ctx = M.getContext();
    IRBuilder<> builder(Ctx);
    Type* voidType = Type::getVoidTy(Ctx);
    Type* int8PtrTy = Type::getInt8Ty(Ctx)->getPointerTo();
    ArrayRef<Type*> logEdgeParamTypes = {int8PtrTy, int8PtrTy};
    FunctionType* logEdgeFuncType = FunctionType::get(voidType, logEdgeParamTypes, false);
    FunctionCallee logEdgeFunc = M.getOrInsertFunction("logEdge", logEdgeFuncType);

    for (auto &F : M) {
      if (F.getName() == "logEdge" || F.isDeclaration()) {
        continue;
      }
      for (auto &B : F) {
        for (auto &I : B) {
          if (llvm::isa<llvm::PHINode>(&I)) {
            continue;
          }
          builder.SetInsertPoint(&I);
          Value* userName = builder.CreateGlobalStringPtr(I.getOpcodeName());

          for (Value *Op : I.operands()) {
            if (auto *Prod = dyn_cast<Instruction>(Op)) {
              if (auto* PN = dyn_cast<PHINode>(Prod)) {
                bool isGood = true;
                for (Value* In : PN->incoming_values()) {
                  if (auto* InI = dyn_cast<Instruction>(In)) {
                    if (isa<PHINode>(InI)) {
                      isGood = false;
                      break;
                    }
                  }
                }
                if (isGood) {
                  for (Value* In : PN->incoming_values()) {
                    if (auto* InI = dyn_cast<Instruction>(In)) {
                      Value* opName = builder.CreateGlobalStringPtr(InI->getOpcodeName());
                      Value* args[] = {userName, opName};
                      builder.CreateCall(logEdgeFunc, args);
                    }
                  }
                } else {
                    Value* opName = builder.CreateGlobalStringPtr(Prod->getOpcodeName());
                    Value* args[] = {userName, opName};
                    builder.CreateCall(logEdgeFunc, args);
                }
              } else {
                Value* opName = builder.CreateGlobalStringPtr(Prod->getOpcodeName());
                Value* args[] = {userName, opName};
                builder.CreateCall(logEdgeFunc, args);
              }
            }
          }
        }
      }
    }
    return PreservedAnalyses::none();
  }
};

PassPluginLibraryInfo getPassPluginInfo() {
  const auto callback = [](PassBuilder &PB) {
    PB.registerOptimizerLastEPCallback(
        [](ModulePassManager &MPM, auto){ MPM.addPass(MyPass{}); return true; });
  };

  return {LLVM_PLUGIN_API_VERSION, "MyPlugin", "0.0.1", callback};
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getPassPluginInfo();
}
