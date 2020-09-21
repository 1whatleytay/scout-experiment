#include <cpu/cpu.h>

#include <fmt/printf.h>

const char *getRegisterName(RegisterIndex index) {
    switch (index) {
        case RegisterIndex::Zero: return "0";
        case RegisterIndex::AssemblerTemporary: return "at";
        case RegisterIndex::ReturnValue0: return "v0";
        case RegisterIndex::ReturnValue1: return "v1";
        case RegisterIndex::Argument0: return "a0";
        case RegisterIndex::Argument1: return "a1";
        case RegisterIndex::Argument2: return "a2";
        case RegisterIndex::Argument3: return "a3";
        case RegisterIndex::Temporary0: return "t0";
        case RegisterIndex::Temporary1: return "t1";
        case RegisterIndex::Temporary2: return "t2";
        case RegisterIndex::Temporary3: return "t3";
        case RegisterIndex::Temporary4: return "t4";
        case RegisterIndex::Temporary5: return "t5";
        case RegisterIndex::Temporary6: return "t6";
        case RegisterIndex::Temporary7: return "t7";
        case RegisterIndex::Saved0: return "s0";
        case RegisterIndex::Saved1: return "s1";
        case RegisterIndex::Saved2: return "s2";
        case RegisterIndex::Saved3: return "s3";
        case RegisterIndex::Saved4: return "s4";
        case RegisterIndex::Saved5: return "s5";
        case RegisterIndex::Saved6: return "s6";
        case RegisterIndex::Saved7: return "s7";
        case RegisterIndex::Temporary8: return "t8";
        case RegisterIndex::Temporary9: return "t9";
        case RegisterIndex::Kernel0: return "k0";
        case RegisterIndex::Kernel1: return "k1";
        case RegisterIndex::GlobalPointer: return "gp";
        case RegisterIndex::StackPointer: return "sp";
        case RegisterIndex::Saved8: return "s8";
        case RegisterIndex::Link: return "lr";
    }
}

void Cpu::unimplemented(const std::string &name, u32 instruction) {
    fmt::print("Unimplemented {} instruction: 0b{:0>32b}.\n", name, instruction);
}

void Cpu::step() {
    u32 instruction = memory.get<u32>(registers.pc);

    u32 op = shift(instruction, 26, 6);

    switch (op) {
        case 0b000000: { // ALU
            u32 func = shift(instruction, 0, 6);
            switch (func) {
                case 0b100000: opAdd(instruction); break;
                case 0b100001: opAddu(instruction); break;
                case 0b100010: opSub(instruction); break;
                case 0b100011: opSubu(instruction); break;
                case 0b011000: opMult(instruction); break;
                case 0b011001: opMultu(instruction); break;
                case 0b011010: opDiv(instruction); break;
                case 0b011011: opDivu(instruction); break;
                case 0b010000: opMfhi(instruction); break;
                case 0b010001: opMthi(instruction); break;
                case 0b010010: opMflo(instruction); break;
                case 0b010011: opMtlo(instruction); break;
                case 0b000000: opSll(instruction); break;
                case 0b000010: opSrl(instruction); break;
                case 0b000011: opSra(instruction); break;
                case 0b000100: opSllv(instruction); break;
                case 0b000110: opSrlv(instruction); break;
                case 0b000111: opSrav(instruction); break;
                case 0b101010: opSlt(instruction); break;
                case 0b101011: opSltu(instruction); break;
                case 0b100100: opAnd(instruction); break;
                case 0b100101: opOr(instruction); break;
                case 0b100110: opXor(instruction); break;
                case 0b100111: opNor(instruction); break;
                case 0b001000: opJr(instruction); break;
                case 0b001001: opJalr(instruction); break;
                case 0b001100: opSyscall(instruction); break;
                case 0b001101: opBreak(instruction); break;
                default: assert(false);
            }
            break;
        }

        case 0b001000: opAddi(instruction); break;
        case 0b001001: opAddiu(instruction); break;
        case 0b001010: opSlti(instruction); break;
        case 0b001011: opSltiu(instruction); break;
        case 0b001100: opAndi(instruction); break;
        case 0b001101: opOri(instruction); break;
        case 0b001110: opXori(instruction); break;
        case 0b001111: opLui(instruction); break;
        case 0b000100: opBeq(instruction); break;
        case 0b000101: opBne(instruction); break;
        case 0b000110: opBlez(instruction); break;
        case 0b000111: opBgtz(instruction); break;
        case 0b010100: opBeql(instruction); break;
        case 0b010101: opBnel(instruction); break;
        case 0b010110: opBlezl(instruction); break;
        case 0b010111: opBgtzl(instruction); break;
        case 0b000010: opJ(instruction); break;
        case 0b000011: opJal(instruction); break;
        case 0b100000: opLb(instruction); break;
        case 0b100001: opLh(instruction); break;
        case 0b100010: opLwl(instruction); break;
        case 0b100011: opLw(instruction); break;
        case 0b100100: opLbu(instruction); break;
        case 0b100101: opLhu(instruction); break;
        case 0b100110: opLwr(instruction); break;
        case 0b101000: opSb(instruction); break;
        case 0b101001: opSh(instruction); break;
        case 0b101010: opSwl(instruction); break;
        case 0b101011: opSw(instruction); break;
        case 0b101110: opSwr(instruction); break;
        case 0b101111: opCache(instruction); break;

        case 0b000001: { // Flow
            u32 func = shift(instruction, 16, 5);
            switch (func) {
                case 0b00000: opBltz(instruction); break;
                case 0b00001: opBgez(instruction); break;
                case 0b10000: opBltzal(instruction); break;
                case 0b10001: opBgezal(instruction); break;
                default: assert(false);
            }
            break;
        }

        case 0b010000: { // COP0
            u32 func = shift(instruction, 21, 5);
            switch (func) {
                case 0b00000: opMtc0(instruction); break;
                case 0b00100: opMfc0(instruction); break;
                default: assert(false);
            }
            break;
        }

        default: {
            assert(false);
        }
    }

    registers.pc += sizeof(instruction);
}

void Cpu::delay(const DelaySlot &slot) {
    slots.push(slot);
}

void Cpu::exec() {
    while (execute) {
        bool hasSlot = !slots.empty();
        step();
        if (hasSlot) {
            slots.front()();
            slots.pop();
        }
    }
}

Cpu::Cpu(const Rom &rom) : memory(rom) {
    registers.regs[static_cast<u8>(RegisterIndex::Saved3)] = 0;
    registers.regs[static_cast<u8>(RegisterIndex::Saved4)] = 1;
    registers.regs[static_cast<u8>(RegisterIndex::Saved5)] = 0;
    registers.regs[static_cast<u8>(RegisterIndex::Saved6)] = 0x3F;
    registers.regs[static_cast<u8>(RegisterIndex::Saved7)] = 0;
    // PIFROM jumps to 0xA4000040
    registers.pc = 0xA4000040;
    // PIFROM initializes SP to 0xA4001FF0 apparently
    registers.regs[static_cast<u8>(RegisterIndex::StackPointer)] = 0xA4001FF0;
}