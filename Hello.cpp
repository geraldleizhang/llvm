//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
using namespace llvm;
using namespace std;

#define DEBUG_TYPE "hello"

STATISTIC(HelloCounter, "Counts number of functions greeted");

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct Hello : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      errs() << "Hello: ";
      errs().write_escaped(F.getName()) << '\n';
      return false;
    }
  };
}

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");

namespace {
  // Hello2 - The second implementation with getAnalysisUsage implemented.
  struct basic : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    basic() : FunctionPass(ID) {}

    int max = 0;
    int min = 100;
    int total = 0;
    int num = 0;

    int total_temp = 0;



    bool runOnFunction(Function &F) override {
      ++HelloCounter;
      int temp = 0;
      int edges = 0;
      
      /*2.1 number of basic blocks*/
      /*Just iterate in the function for each blocks and count it*/

      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); ++it){
        temp ++;
        //cout<<(*it).size()<<endl;
        TerminatorInst *T = it->getTerminator();
        int succ = T->getNumSuccessors();
        //for (int i=0; i<succ; i++){
        //  errs() << T->getSuccessor(i);
        //}
        //errs() << succ <<'\n';
        edges += succ;
      }
      //errs() << edges<<'\n';
      if (temp > max)
        max = temp;
      if (temp < min)
        min = temp;
      total += temp;
      num ++;

      //errs() << max <<" " <<min <<" "<<" "<< total <<" "<<total/num<<'\n';

      /*2.2 number of CFG edges*/
      /*For each block, check the number of its successors, which is the number of edges*/
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); ++it){
        temp ++;
        //cout<<(*it).size()<<endl;
        TerminatorInst *T = it->getTerminator();
        int succ = T->getNumSuccessors();
        //for (int i=0; i<succ; i++){
        //  errs() << T->getSuccessor(i);
        //}
        //errs() << succ <<'\n';
        edges += succ;
      }
      //errs() << edges <<'\n';

      /*2.3 number of single entry loops*/
      /*If there is a back edge, that is: A dominates B and A is a successor of B, then there is a loop*/

      int loopnum = 0;

      DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
      //Function::iterator it = F.begin();
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); it++){
        //BasicBlock *BO = &*it;
        TerminatorInst *T = it -> getTerminator();
        int succ = T->getNumSuccessors();
        for(int i=0; i<succ; i++){
          BasicBlock *B1 = T->getSuccessor(i);
          if(DT->dominates( B1, (BasicBlock*) it)) loopnum++;
        }
      }
      //errs()<<loopnum<<'\n';
      //BasicBlock *B0 = &*it++;
      //BasicBlock *B1 = &*it++;
      //errs() << DT->dominates(B1, B0)<<'\n';
      


      
      /*2.4 number of loop basic blocks*/
      /*Just iterate for each block to check if it belongs to a loop*/
      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

      //LI.print(errs());
      int loopcount = 0;
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); it++){
        if((&LI)->getLoopFor((BasicBlock*)it)) loopcount++;
      }

      errs()<<loopcount<<'\n';

      int dom_num = 0;
      int dom_total = 0;
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); it++){
        for (Function::iterator its = (&F)->begin(); its!=(&F)->end(); its++){
          if( DT->dominates(  (BasicBlock*)it, (BasicBlock*)its  ) & (it != its)) dom_total++;
        }
        dom_num++;
      }
      total_temp += float(dom_total)/float(dom_num);

      errs()<< dom_total <<" "<< dom_num <<" "<<float(total_temp)/float(num)<<'\n';

      return false;
    }



    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.addRequired<LoopInfoWrapperPass>();
    }
  };
}

char basic::ID = 0;
static RegisterPass<basic>
Y("basic", "Hello World Pass (with getAnalysisUsage implemented)");
//INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)


namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct part3 : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    part3() : FunctionPass(ID) {}
    int loopnum = 0;

    bool runOnFunction(Function &F) override {
      ++HelloCounter;

      /*3.1.1*/

      DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
      //Function::iterator it = F.begin();
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); it++){
        //BasicBlock *BO = &*it;
        TerminatorInst *T = it -> getTerminator();
        int succ = T->getNumSuccessors();
        for(int i=0; i<succ; i++){
          BasicBlock *B1 = T->getSuccessor(i);
          if(DT->dominates( B1, (BasicBlock*) it)) loopnum++;
        }
      }
      //errs() << loopnum<<'\n';
      /*3.1.2*/
      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      int outermostloop = 0;
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); it++){
        //here I count if LoopDepth is 1.
        if ((&LI)->getLoopDepth((BasicBlock*)it) == 1) outermostloop ++;
      }
      //errs() << outermostloop<<'\n';

      /*3.1.3*/
      int edges = 0;
      for (Function::iterator it = (&F)->begin(); it!=(&F)->end(); ++it){
        //cout<<(*it).size()<<endl;
        TerminatorInst *T = it->getTerminator();
        int succ = T->getNumSuccessors();
        for (int i=0; i<succ; i++){
          BasicBlock* blk = T->getSuccessor(i);
          if ( ((&LI) ->getLoopDepth((BasicBlock*)it) > 0) & (  (&LI)->getLoopDepth(blk) == 0 )     )
            //errs() << (&LI) ->getLoopDepth((BasicBlock*)it)  << ' '<<   (&LI)->getLoopDepth(blk)  <<'\n';
            edges++;
        }
        //errs() << succ <<'\n';
        //edges += succ;
      }
      errs() << edges<<'\n';



      return false;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.addRequired<LoopInfoWrapperPass>();
    }
  };
}

char part3::ID = 0;
static RegisterPass<part3> Z("part3", "Hello World Pass");


