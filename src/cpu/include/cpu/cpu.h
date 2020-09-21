#pragma once

#include <cpu/memory.h>

#include <queue>

enum class RegisterIndex : u8 {
    Zero,
    AssemblerTemporary,
    ReturnValue0,
    ReturnValue1,
    Argument0,
    Argument1,
    Argument2,
    Argument3,
    Temporary0,
    Temporary1,
    Temporary2,
    Temporary3,
    Temporary4,
    Temporary5,
    Temporary6,
    Temporary7,
    Saved0,
    Saved1,
    Saved2,
    Saved3,
    Saved4,
    Saved5,
    Saved6,
    Saved7,
    Temporary8,
    Temporary9,
    Kernel0,
    Kernel1,
    GlobalPointer,
    StackPointer,
    Saved8,
    Link,
};

const char *getRegisterName(RegisterIndex index);

class Registers {
public:
    i64 regs[32] = {0};

    i64 hi = 0;
    i64 lo = 0;
    u64 pc = 0;
    u64 llb = 0;
};

typedef std::function<void()> DelaySlot;

class Cpu {
    Registers registers;
    Memory memory;

    std::queue<DelaySlot> slots;

    static void unimplemented(const std::string &name, u32 instruction);

    void delay(const DelaySlot &slot);

    // ALU
    void opAdd(u32 instruction);
    void opAddu(u32 instruction);
    void opSub(u32 instruction);
    void opSubu(u32 instruction);
    void opMult(u32 instruction);
    void opMultu(u32 instruction);
    void opDiv(u32 instruction);
    void opDivu(u32 instruction);
    void opMfhi(u32 instruction);
    void opMthi(u32 instruction);
    void opMflo(u32 instruction);
    void opMtlo(u32 instruction);
    void opSll(u32 instruction);
    void opSrl(u32 instruction);
    void opSra(u32 instruction);
    void opSllv(u32 instruction);
    void opSrlv(u32 instruction);
    void opSrav(u32 instruction);
    void opSlt(u32 instruction);
    void opSltu(u32 instruction);
    void opAnd(u32 instruction);
    void opOr(u32 instruction);
    void opXor(u32 instruction);
    void opNor(u32 instruction);
    void opSyscall(u32 instruction);
    void opBreak(u32 instruction);

    // Immediate
    void opAddi(u32 instruction);
    void opAddiu(u32 instruction);
    void opSlti(u32 instruction);
    void opSltiu(u32 instruction);
    void opAndi(u32 instruction);
    void opOri(u32 instruction);
    void opXori(u32 instruction);
    void opLui(u32 instruction);

    // Flow
    void opBeq(u32 instruction);
    void opBne(u32 instruction);
    void opBlez(u32 instruction);
    void opBgtz(u32 instruction);
    void opBltz(u32 instruction);
    void opBgez(u32 instruction);
    void opBltzal(u32 instruction);
    void opBgezal(u32 instruction);
    void opBeql(u32 instruction);
    void opBnel(u32 instruction);
    void opBlezl(u32 instruction);
    void opBgtzl(u32 instruction);
    void opJ(u32 instruction);
    void opJal(u32 instruction);
    void opJr(u32 instruction);
    void opJalr(u32 instruction);
    void opCache(u32 instruction);

    // Data
    void opLb(u32 instruction);
    void opLh(u32 instruction);
    void opLwl(u32 instruction);
    void opLw(u32 instruction);
    void opLbu(u32 instruction);
    void opLhu(u32 instruction);
    void opLwr(u32 instruction);
    void opSb(u32 instruction);
    void opSh(u32 instruction);
    void opSwl(u32 instruction);
    void opSw(u32 instruction);
    void opSwr(u32 instruction);

    // COP 0
    void opMtc0(u32 instruction);
    void opMfc0(u32 instruction);

    void step();

public:
    volatile bool execute = true;

    void exec();

    explicit Cpu(const Rom &rom);
};