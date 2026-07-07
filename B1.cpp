#include<array>
#include<cstdint>
#include<cstdio>
#include<cstdlib>

uint16_t pc = 0x0000, lpc = 0x0000; // The cashe ones, no ram one since its a custom ISO
uint8_t rA = 0x00, rB = 0x00, rC = 0x00; // Registors

constexpr size_t RAM = 1ull << 24; // 16mb of ram
uint8_t ram[RAM];
constexpr size_t I_CASHE = 1ull << 16; // 64kb of istuction cashe
uint8_t i_cashe[I_CASHE];
constexpr size_t D_CASHE = 1ull << 8; // 256B of data cashe
uint8_t d_cashe[D_CASHE];

int program_load(){
    char* test_bcode_path = "test_program.bcd";
    uint8_t test_program[] = { //with bcode syntax
        //.loader(
        0x11, 0x00, 0x00, 0x0d,
        0xa0, 0x00,
        //#entry loop
        //)
        //.loop(
        0x08,
        0xb0,
        0xa3,
        0xfe,
        0x09, 0x0a, //loop check, jump
        0xff,
        //)
        //<data>(
        0x01
        //)
    };
    if(test_bcode_path == nullptr){
        printf("No test program path provided, loading default test program\n");
        size_t program_size = sizeof(test_program);
        for(size_t i = 0; i < program_size; ++i){
            ram[i] = test_program[i];
        };
    }else{
          printf("Loading test program from %s //later\n", test_bcode_path);
    };
    return 0;
};

void crash(){
    exit(-1);
}

int d_cashe_load(uint32_t addr){
    printf("%u\n", addr);
    if(addr + D_CASHE > RAM) crash();
    for(size_t i = 0; i < D_CASHE; ++i){
        d_cashe[i] = ram[addr + i];
        if(!d_cashe[i] == 0){
            printf("d/%u\n", d_cashe[i]);
        };
    };
    return 0;
};

int i_cashe_load(uint32_t addr){
    printf("%u\n", addr);
    if(addr + I_CASHE > RAM) crash();
    for(size_t i = 0; i < I_CASHE; ++i){
        i_cashe[i] = ram[addr + i];
    };
    return 0;
};

int cashe_reset(uint8_t res_val){
    for(size_t i = 0; i < D_CASHE; ++i){
        d_cashe[i] = res_val;
    };
    for(size_t i = 0; i < I_CASHE; ++i){
        i_cashe[i] = res_val;
    };
    return 0;
}

int d_cashe_save(uint32_t addr){
    for(size_t i = 0; i < D_CASHE; ++i){
    ram[addr + i] = d_cashe[i];
    };
    return 0;
};

uint32_t Get_bytes_3(){
    ++pc;
    uint8_t high = i_cashe[pc];
    ++pc;
    uint8_t med = i_cashe[pc];
    ++pc;
    uint8_t low = i_cashe[pc];
    return ((uint32_t)high << 16) | ((uint32_t)med << 8) | low;
};

uint16_t Get_bytes_2(){
    ++pc;
    uint8_t high = i_cashe[pc];
    ++pc;
    uint8_t low = i_cashe[pc];
    return (((uint16_t)high << 8) | low);
};

int emulator(){
    program_load();
    cashe_reset(0);
    i_cashe_load(0);
    while(true){
        switch(i_cashe[pc]){
            case 0x07:{
            pc = Get_bytes_2();
                --pc;
                break;
            };
            case 0x08:{
                lpc = pc;
                printf("lsp %u\n", lpc);
                break;
            };
            case 0x09:{
                ++pc;
                uint8_t t = i_cashe[pc];
                printf("%u\n", t);
                if(t != rC){
                    pc = lpc;
                    --pc;
                    printf("lop fail rC = %u /=/ %u\n", t, rC);
                }else{
                    printf("lop pass -> rC = %u\n", t);
                };
                break;
            };
            case 0x10:{
                printf("icl ");
                i_cashe_load(Get_bytes_3());
                break;
            };
            case 0x11:{
                printf("dcl ");
                d_cashe_load(Get_bytes_3());
                break;
            };
            case 0x12:{
                break;
            };
            case 0xa0:{
                ++pc;
                rA = d_cashe[i_cashe[pc]];
                printf("lra %u\n", rA);
                break;
            };
            case 0xa1:{
                ++pc;
                rB = d_cashe[i_cashe[pc]];
                printf("lrb %u\n", rB);
                break;
            };
            case 0xa2:{
                rA ^= rB;
                rB ^= rA;
                rA ^= rB;
                printf("swp\nrA %u\nrB %u\n", rA, rB);
                break;
            };
            case 0xa3:{
                printf("crc\n");
                rB = rC;
                break;
            };
            case 0xb0:{
                rC = rA + rB;
                printf("add %u\n", rC);
                break;
            };
            case 0xb1:{
                printf("min\n");
                rC = rA - rB;
                break;
            };
            case 0xb2:{
                printf("mul\n");
                rC = rA * rB;
                break;
            };
            case 0xb3:{
                printf("diw\n");
                rC = rA / rB;
                break;
            };
            case 0xfe:{
                printf("rC= %u\n", rC);
                break;
            };
            case 0xff:{
                printf("ext\n");
                return 0;
                break;
            };
        
        };
        ++pc;
        printf("pc %u\n", pc);
        getchar();
    };
};

int main(){
    emulator();
    return 0;
};
