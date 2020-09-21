#include <cpu/cpu.h>

#include <fmt/printf.h>

#define REGNAME(index) \
    fmt::format("${}", getRegisterName(static_cast<RegisterIndex>(index)))
#define REGFMT(index, text) \
    fmt::format("{}@{}", REGNAME(index), fmt::format(text, static_cast<u64>(registers.regs[index])))
#define REGVAL(index) REGFMT(index, "{}")
#define REGBRANCH(value) \
    fmt::format(hex, registers.pc + value * sizeof(u32))
#define DISASM(name, text, ...) \
    fmt::print("[0x{:0>8X}] 0x{:0>8X}: {} {}\n", registers.pc, instruction, name, fmt::format(text, __VA_ARGS__))

//#undef DISASM
//#define DISASM(...)

const char *hex = "0x{:0>8X}";
const char *bin = "0b{:0>32b}";

void Cpu::opAdd(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 11, 5);

    DISASM("add", "{}, {} + {}", REGNAME(dest), REGVAL(a), REGVAL(b));

    registers.regs[dest] = registers.regs[a] + registers.regs[b];
}
void Cpu::opAddu(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 11, 5);

    DISASM("addu", "{}, {} + {}", REGNAME(dest), REGVAL(a), REGVAL(b));

    registers.regs[dest] = registers.regs[a] + registers.regs[b];
}
void Cpu::opSub(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 11, 5);

    DISASM("sub", "{}, {} - {}", REGNAME(dest), REGVAL(a), REGVAL(b));

    registers.regs[dest] = registers.regs[a] - registers.regs[b];
}
void Cpu::opSubu(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 11, 5);

    DISASM("subu", "{}, {} - {}", REGNAME(dest), REGVAL(a), REGVAL(b));

    registers.regs[dest] = registers.regs[a] - registers.regs[b];
}
void Cpu::opMult(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);

    DISASM("mult", "{} * {}", REGVAL(a), REGVAL(b));

    u64 result = static_cast<i64>(static_cast<i32>(registers.regs[a])) * static_cast<i32>(registers.regs[b]);
    registers.lo = shift(result, 0, 32);
    registers.hi = shift(result, 32, 32);
}
void Cpu::opMultu(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);

    DISASM("multu", "{} * {}", REGVAL(a), REGVAL(b));

    u64 result = static_cast<u64>(static_cast<u32>(registers.regs[a])) * static_cast<u32>(registers.regs[b]);
    registers.lo = shift(result, 0, 32);
    registers.hi = shift(result, 32, 32);
}
void Cpu::opDiv(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);

    DISASM("div", "{} / {}", REGVAL(a), REGVAL(b));

    u32 result = static_cast<i32>(registers.regs[a]) / static_cast<i32>(registers.regs[b]);
    u32 remainder = static_cast<i32>(registers.regs[a]) % static_cast<i32>(registers.regs[b]);
    registers.lo = result;
    registers.hi = remainder;
}
void Cpu::opDivu(u32 instruction) {
    u8 a = shift(instruction, 21, 5);
    u8 b = shift(instruction, 16, 5);

    DISASM("divu", "{} / {}", REGVAL(a), REGVAL(b));

    u32 result = static_cast<u32>(registers.regs[a]) / static_cast<u32>(registers.regs[b]);
    u32 remainder = static_cast<u32>(registers.regs[a]) % static_cast<u32>(registers.regs[b]);
    registers.lo = result;
    registers.hi = remainder;
}
void Cpu::opMfhi(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);

    DISASM("mfhi", "{}, $HI", REGNAME(dest));

    registers.regs[dest] = registers.hi;
}
void Cpu::opMthi(u32 instruction) {
    u8 src = shift(instruction, 21, 5);

    DISASM("mthi", "$HI, {}", REGFMT(src, hex));

    registers.hi = registers.regs[src];
}
void Cpu::opMflo(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);

    DISASM("mflo", "{}, $LO", REGNAME(dest));

    registers.regs[dest] = registers.lo;
}
void Cpu::opMtlo(u32 instruction) {
    u8 src = shift(instruction, 21, 5);

    DISASM("mtlo", "$LO, {}", REGFMT(src, hex));

    registers.lo = registers.regs[src];
}
void Cpu::opSll(u32 instruction) {
    u8 move = shift(instruction, 6, 5);
    u8 dest = shift(instruction, 11, 5);
    u8 src = shift(instruction, 16, 5);

    if (instruction == 0)
        DISASM("nop", "", "");
    else
        DISASM("sll", "{}, {} << {}", REGNAME(dest), REGFMT(src, hex), move);

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) << move;
}
void Cpu::opSrl(u32 instruction) {
    u8 move = shift(instruction, 6, 5);
    u8 dest = shift(instruction, 11, 5);
    u8 src = shift(instruction, 16, 5);

    DISASM("srl", "{}, {} >> {}", REGNAME(dest), REGFMT(src, hex), move);

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) >> move;
}
void Cpu::opSra(u32 instruction) {
    u8 move = shift(instruction, 6, 5);
    u8 dest = shift(instruction, 11, 5);
    u8 src = shift(instruction, 16, 5);

    DISASM("sra", "{}, {} >> {}", REGNAME(dest), REGFMT(src, hex), move);
    unimplemented("sra: doesn't use proper i32 for shift, see warning", instruction);

    registers.regs[dest] = static_cast<u32>(registers.regs[src]) >> move;
}
void Cpu::opSllv(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("sllv", "{}, {} << {}", REGNAME(dest), REGFMT(a, hex), REGVAL(b));

    registers.regs[dest] = static_cast<u64>(registers.regs[a]) << static_cast<u64>(registers.regs[b]);
}
void Cpu::opSrlv(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("srlv", "{}, {} >> {}", REGNAME(dest), REGFMT(a, hex), REGVAL(b));

    registers.regs[dest] = static_cast<u64>(registers.regs[a]) >> static_cast<u64>(registers.regs[b]);
}
void Cpu::opSrav(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("srav", "{}, {} >> {}", REGNAME(dest), REGFMT(a, hex), REGVAL(b));
    unimplemented("srav: doesn't use proper i32 for shift, see warning", instruction);

    registers.regs[dest] = static_cast<u32>(registers.regs[a]) >> static_cast<u64>(registers.regs[b]);
}
void Cpu::opSlt(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("slt", "{}, {} < {}", REGNAME(dest), REGVAL(a), REGVAL(b));

    registers.regs[dest] = registers.regs[b] < registers.regs[a];
}
void Cpu::opSltu(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("sltu", "{}, {} < {}", REGNAME(dest), REGVAL(b), REGVAL(a));

    registers.regs[dest] = static_cast<u64>(registers.regs[b]) < static_cast<u64>(registers.regs[a]);
}
void Cpu::opAnd(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("and", "{}, {} & {}", REGNAME(dest), REGFMT(a, hex), REGFMT(b, hex));

    registers.regs[dest] = static_cast<u64>(registers.regs[a]) & static_cast<u64>(registers.regs[b]);
}
void Cpu::opOr(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("or", "{}, {} | {}", REGNAME(dest), REGFMT(a, hex), REGFMT(b, hex));

    registers.regs[dest] = static_cast<u64>(registers.regs[a]) | static_cast<u64>(registers.regs[b]);
}
void Cpu::opXor(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("xor", "{}, {} ^ {}", REGNAME(dest), REGFMT(a, hex), REGFMT(b, hex));

    registers.regs[dest] = static_cast<u64>(registers.regs[a]) ^ static_cast<u64>(registers.regs[b]);
}
void Cpu::opNor(u32 instruction) {
    u8 dest = shift(instruction, 11, 5);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("nor", "{}, ~({} | {})", REGNAME(dest), REGFMT(a, hex), REGFMT(b, hex));

    registers.regs[dest] = ~(static_cast<u64>(registers.regs[a]) | static_cast<u64>(registers.regs[b]));
}
void Cpu::opSyscall(u32 instruction) {
    unimplemented("Syscall", instruction);
}
void Cpu::opBreak(u32 instruction) {
    unimplemented("Break", instruction);
}
void Cpu::opAddi(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("addi", "{}, {} + {}", REGNAME(dest), REGVAL(src), value);

    registers.regs[dest] = registers.regs[src] + value;
}
void Cpu::opAddiu(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("addiu", "{}, {} + {}", REGNAME(dest), REGVAL(src), value);

    registers.regs[dest] = registers.regs[src] + value;
}
void Cpu::opSlti(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("slti", "{}, {} < {}", REGNAME(dest), REGVAL(src), value);

    registers.regs[dest] = registers.regs[src] < value;
}
void Cpu::opSltiu(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("sltiu", "{}, {} < {}", REGNAME(dest), REGVAL(src), value);

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) < value;
}
void Cpu::opAndi(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("andi", "{}, {} & {}", REGNAME(dest), REGFMT(src, hex), fmt::format(hex, value));

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) & value;
}
void Cpu::opOri(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("ori", "{}, {} | {}", REGNAME(dest), REGFMT(src, hex), fmt::format(hex, value));

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) | value;
}
void Cpu::opXori(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("xori", "{}, {} ^ {}", REGNAME(dest), REGFMT(src, hex), fmt::format(hex, value));

    registers.regs[dest] = static_cast<u64>(registers.regs[src]) ^ value;
}
void Cpu::opLui(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);

    DISASM("lui", "{}, {}", REGNAME(dest), fmt::format(hex, static_cast<u64>(value) << 16u));

//    u64 mask = ~(~0ull >> 16u << 16u << 32u >> 32u); // 0xFFFFFFFF0000FFFF
//    u64 temp = (static_cast<u64>(registers.regs[dest]) & mask) | (static_cast<u64>(value) << 16u);

    registers.regs[dest] = static_cast<u64>(value) << 16u;
}
void Cpu::opBeq(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("beq", "{}, {} == {}", REGBRANCH(value), REGVAL(a), REGVAL(b));

    if (registers.regs[a] == registers.regs[b]) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBne(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("bne", "{}, {} != {}", REGBRANCH(value), REGVAL(a), REGVAL(b));

    if (registers.regs[a] != registers.regs[b]) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBlez(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("blez", "{}, {} <= 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] <= 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBgtz(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bgtz", "{}, {} > 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] > 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBltz(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bltz", "{}, {} < 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] < 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBgez(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bgez", "{}, {} >= 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] >= 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBltzal(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bltzal", "{}, {} < 0 (&link)", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] < 0) {
        delay([this, value]() {
            registers.regs[static_cast<u8>(RegisterIndex::Link)] = registers.pc + sizeof(u32);
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBgezal(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bgezal", "{}, {} >= 0 (&link)", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] >= 0) {
        delay([this, value]() {
            registers.regs[static_cast<u8>(RegisterIndex::Link)] = registers.pc + sizeof(u32);
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBeql(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("beql", "{}, {} == {}", REGBRANCH(value), REGVAL(a), REGVAL(b));

    if (registers.regs[a] == registers.regs[b]) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBnel(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 a = shift(instruction, 16, 5);
    u8 b = shift(instruction, 21, 5);

    DISASM("bnel", "{}, {} != {}", REGBRANCH(value), REGVAL(a), REGVAL(b));

    if (registers.regs[a] != registers.regs[b]) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBlezl(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("blezl", "{}, {} <= 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] <= 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opBgtzl(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 21, 5);

    DISASM("bgtzl", "{}, {} > 0", REGBRANCH(value), REGVAL(src));

    if (registers.regs[src] > 0) {
        delay([this, value]() {
            registers.pc += (value - 1) * sizeof(u32);
        });
    }
}
void Cpu::opJ(u32 instruction) {
    u32 value = shift(instruction, 0, 26);
    u32 address = (registers.pc & 0xF0000000u) | (value * sizeof(u32));

    DISASM("j", "{}", fmt::format(hex, address));

    delay([this, address]() {
        registers.pc = address;
    });
}
void Cpu::opJal(u32 instruction) {
    u32 value = shift(instruction, 0, 26);
    u32 address = (registers.pc & 0xF0000000u) | (value * sizeof(u32));

    DISASM("jal", "{} (&link)", fmt::format(hex, address));

    delay([this, address]() {
        registers.regs[static_cast<u8>(RegisterIndex::Link)] = registers.pc;
        registers.pc = address;
    });
}
void Cpu::opJr(u32 instruction) {
    u8 src = shift(instruction, 21, 5);

    DISASM("jr", "{}", REGFMT(src, hex));

    delay([this, src]() {
        registers.pc = registers.regs[src];
    });
}
void Cpu::opJalr(u32 instruction) {
    u8 src = shift(instruction, 21, 5);
    u8 dest = shift(instruction, 11, 5);

    DISASM("jalr", "{} (&{})", REGFMT(src, hex), REGNAME(dest));

    delay([this, src, dest]() {
        registers.regs[dest] = registers.pc;
        registers.pc = registers.regs[src];
    });
}
void Cpu::opCache(u32 instruction) {
    unimplemented("Cache", instruction);
}
void Cpu::opLb(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("lb", "{}, [{} + {}]", REGNAME(dest), REGFMT(src, hex), value);

    registers.regs[dest] = memory.getByte(registers.regs[src] + value);
}
void Cpu::opLh(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("lh", "{}, [{} + {}]", REGNAME(dest), REGFMT(src, hex), value);

    registers.regs[dest] = memory.get<u16>(registers.regs[src] + value);
}
void Cpu::opLwl(u32 instruction) {
    unimplemented("Lwl", instruction);
}
void Cpu::opLw(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("lw", "{}, [{} + {}]", REGNAME(dest), REGFMT(src, hex), value);

    registers.regs[dest] = memory.get<u32>(registers.regs[src] + value);
}
void Cpu::opLbu(u32 instruction) {
    u16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("lbu", "{}, [{} + {}]", REGNAME(dest), REGFMT(src, hex), value);

    registers.regs[dest] = memory.getByte(registers.regs[src] + value);
}
void Cpu::opLhu(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 dest = shift(instruction, 16, 5);
    u8 src = shift(instruction, 21, 5);

    DISASM("lhu", "{}, [{} + {}]", REGNAME(dest), REGFMT(src, hex), value);

    registers.regs[dest] = memory.get<u16>(registers.regs[src] + value);
}
void Cpu::opLwr(u32 instruction) {
    unimplemented("Lwr", instruction);
}
void Cpu::opSb(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 21, 5);

    DISASM("sb", "[{} + {}], {}", REGFMT(dest, hex), value, REGFMT(src, hex));

    memory.setByte(registers.regs[dest] + value, registers.regs[src]);
}
void Cpu::opSh(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 21, 5);

    DISASM("sh", "[{} + {}], {}", REGFMT(dest, hex), value, REGFMT(src, hex));

    memory.set<u16>(registers.regs[dest] + value, registers.regs[src]);
}
void Cpu::opSwl(u32 instruction) {
    unimplemented("Swl", instruction);
}
void Cpu::opSw(u32 instruction) {
    i16 value = shift(instruction, 0, 16);
    u8 src = shift(instruction, 16, 5);
    u8 dest = shift(instruction, 21, 5);

    DISASM("sw", "[{} + {}], {}", REGFMT(dest, hex), value, REGFMT(src, hex));

    memory.set<u32>(registers.regs[dest] + value, registers.regs[src]);
}
void Cpu::opSwr(u32 instruction) {
    unimplemented("Swr", instruction);
}
void Cpu::opMtc0(u32 instruction) {
    unimplemented("Mtc0", instruction);
}
void Cpu::opMfc0(u32 instruction) {
    unimplemented("Mfc0", instruction);
}