#include <stdio.h>
#include "shell.h"

void R_Type(uint32_t instruction);
void J_Type(uint32_t instruction);
void I_Type(uint32_t instruction);

/*J指令op字段即可判断*/
#define J 0x2
#define JAL 0x3

/*R型指令op字段都为0，还需要funct字段进一步判断*/
#define SLL 0x0
#define SRL 0x2
#define SRA 0x3
#define SLLV 0x4
#define SRLV 0x6
#define SRAV 0x7
#define JR 0x8
#define JALR 0x9
#define SYSCALL 0xC
#define MFHI 0x10
#define MTHI 0x11
#define MFLO 0x12
#define MTLO 0x13
#define MULT 0x18
#define MULTU 0x19
#define DIV	0x1A
#define DIVU 0x1B
#define ADD	0x20
#define ADDU 0x21
#define SUB	0x22
#define SUBU 0x23
#define AND	0x24
#define OR 0x25
#define XOR	0x26
#define NOR	0x27
#define SLT 0x2A
#define SLTU 0x2B

/*I型指令op字段即可判断*/
//1.有几个特殊的条件跳转op一样都是1，那么根据rt来确定具体的指令
#define REGIMM 0x1
#define BLTZ 0x0
#define BGEZ 0x1
#define BLTZAL 0x10
#define BGEZAL 0x11
//2.其它的I型指令
#define BEQ	0x4
#define BNE	0x5
#define BLEZ 0x6
#define BGTZ 0x7
#define ADDI 0x8
#define ADDIU 0x9
#define SLTI 0xA
#define SLTIU 0xB
#define ANDI 0xC
#define ORI	0xD
#define XORI 0xE
#define LUI	0xF
#define LB 0x20
#define LH 0x21
#define LW 0x23
#define LBU	0x24
#define LHU	0x25
#define SB 0x28
#define SH 0x29
#define SW 0x2B

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction=0x0;
	instruction=mem_read_32(CURRENT_STATE.PC);
    //取op段
    uint8_t op=0x0;
    op=(instruction>>26)&0x3F;

    switch(op)
    {
        case 0x0:
        R_Type(instruction);
        break;

        case 0x2:
        case 0x3:
        J_Type(instruction);
        break;

        default:
        I_Type(instruction);
    }

}

void R_Type(uint32_t instruction)
{
    //首先对R型指令进行解析
    //op6位，rs,rt,rd,shamt5位，funct6位，这里都用unit8_t保存
    uint8_t op=0,rs=0,rt=0,rd=0,shamt=0,funct=0;
    op=(instruction>>26)&0x3F;
	rs=(instruction>>21)&0x1F;
	rt=(instruction>>16)&0x1F;
    rd=(instruction>>11)&0x1F;
	shamt=(instruction>>6)&0x1F;
	funct=instruction&0x3F;

    switch(funct)
    {
        case SLL:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<shamt;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SRL:
        //补0
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>shamt;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SRA:
        //补符号位
        NEXT_STATE.REGS[rd]=(int32_t)CURRENT_STATE.REGS[rt]>>shamt;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SLLV:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]<<(CURRENT_STATE.REGS[rs]&0x000001F);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;       

        case SRLV:
        //补0
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rt]>>(CURRENT_STATE.REGS[rs]&0x000001F);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SRAV:
        //补符号位
        NEXT_STATE.REGS[rd]=(int32_t)(CURRENT_STATE.REGS[rt])>>(CURRENT_STATE.REGS[rs]&0x000001F);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case JR:
        NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
        break;

        case JALR:
        NEXT_STATE.PC=CURRENT_STATE.REGS[rs];
		NEXT_STATE.REGS[rd]=CURRENT_STATE.PC+4;
        break;

        case SYSCALL:
        if (CURRENT_STATE.REGS[2]==0x0000000A) {
		  RUN_BIT=0;
        } 
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case MFHI:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.HI;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;

        case MTHI:
        NEXT_STATE.HI=CURRENT_STATE.REGS[rs];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case MFLO:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.LO;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;        

        case MTLO:
        NEXT_STATE.LO=CURRENT_STATE.REGS[rs];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case MULT:
	    int64_t mult=((int64_t)CURRENT_STATE.REGS[rs])*((int64_t)CURRENT_STATE.REGS[rt]);
        NEXT_STATE.HI=(mult>>32)&0xFFFFFFFF;
	    NEXT_STATE.LO=(mult>>0)&0xFFFFFFFF; 
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case MULTU:
	    uint64_t multu=CURRENT_STATE.REGS[rs]*CURRENT_STATE.REGS[rt];
        NEXT_STATE.HI=(multu>>32)&0xFFFFFFFF;
	    NEXT_STATE.LO=(multu>>0)&0xFFFFFFFF; 
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;   

        case DIV:
        NEXT_STATE.HI=((int32_t)CURRENT_STATE.REGS[rs]%(int32_t)CURRENT_STATE.REGS[rt]);
		NEXT_STATE.LO=((int32_t)CURRENT_STATE.REGS[rs]/(int32_t)CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;

        case DIVU:
        NEXT_STATE.HI=(CURRENT_STATE.REGS[rs]%CURRENT_STATE.REGS[rt]);
		NEXT_STATE.LO=(CURRENT_STATE.REGS[rs]/CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;  

        case ADD:
	    NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+CURRENT_STATE.REGS[rt];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;      
		break;

        case ADDU:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]+CURRENT_STATE.REGS[rt];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;      
		break;

        case SUB:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;      
		break;    

        case SUBU:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]-CURRENT_STATE.REGS[rt];
	    NEXT_STATE.PC=CURRENT_STATE.PC+4;      
		break;

        case AND:
		NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]&CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC=CURRENT_STATE.PC+4;  
		break;
			
		case OR:
		NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;
			
		case XOR:
		NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]^CURRENT_STATE.REGS[rt];
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;
			
		case NOR:
		NEXT_STATE.REGS[rd]=~(CURRENT_STATE.REGS[rs]|CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
		break;  

        case SLT:
        NEXT_STATE.REGS[rd]=((int32_t)CURRENT_STATE.REGS[rs])<((int32_t)CURRENT_STATE.REGS[rt])?1:0;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;


        case SLTIU:
        NEXT_STATE.REGS[rd]=CURRENT_STATE.REGS[rs]<CURRENT_STATE.REGS[rt]?1:0;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

    }

}

void I_Type(uint32_t instruction)
{
    //首先对I型指令进行解析
    //op6位,rs,rt5位,imm16位，其中op,rs,rt用uint8_t存,imm用uint16_t存
    uint8_t op=0,rs=0,rt=0,rd=0;
	uint16_t imm=0;
	op=(instruction>>26)&0x3F;
	rs=(instruction>>21)&0x1F;
	rt=(instruction>>16)&0x1F;
	imm=instruction&0x0000FFFF;

    int32_t offset=0;
    int32_t extend_imm=0;
    uint32_t u_extend_imm=0;
    uint32_t address=0;

    switch(op)
    {
        case REGIMM:
            offset=(imm<<16)>>14;
            switch(rt)
            {

            case BLTZ:
            //rs的最高位是否为1
            if ((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000) {
                NEXT_STATE.PC=CURRENT_STATE.PC+offset;
                //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
            } 
            else {
                NEXT_STATE.PC=CURRENT_STATE.PC + 4;
            }
            break;

            case BGEZ:
            //rs的最高位是否为0
            if (!((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000)) {
                NEXT_STATE.PC=CURRENT_STATE.PC+offset;
                //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
            } 
            else {
                NEXT_STATE.PC=CURRENT_STATE.PC + 4;
            }
            break;

            case BLTZAL:
            //rs的最高位是否为1
            if ((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000) {
                NEXT_STATE.PC=CURRENT_STATE.PC+offset;
                //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
            } 
            else {
                NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            CURRENT_STATE.REGS[31]=CURRENT_STATE.PC+4;
            break;

            case BGEZAL:
            //rs的最高位是否为0
            if (!((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000)) {
                NEXT_STATE.PC=CURRENT_STATE.PC+offset;
                //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
            } 
            else {
              NEXT_STATE.PC=CURRENT_STATE.PC+4;
            }
            CURRENT_STATE.REGS[31]=CURRENT_STATE.PC+4;
            break;
        }
        break;

        case BEQ:
        offset=(imm<<16)>>14;
        if(CURRENT_STATE.REGS[rs]==CURRENT_STATE.REGS[rt]) {
            NEXT_STATE.PC=CURRENT_STATE.PC+offset;
            //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
        }
        else {
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }
        break;

        case BNE:
        offset=(imm<<16)>>14;
        if(CURRENT_STATE.REGS[rs]!=CURRENT_STATE.REGS[rt]) {
            NEXT_STATE.PC=CURRENT_STATE.PC+offset;
            //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
        }
        else {
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }      
        break;

        case BLEZ:
        offset=(imm<<16)>>14;
        if((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000||CURRENT_STATE.REGS[rs]==0){
            NEXT_STATE.PC=CURRENT_STATE.PC+offset;
            //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
        }
        else {
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }
		break;

        case BGTZ:
        offset=(imm<<16)>>14;
        if(!((CURRENT_STATE.REGS[rs]&0x80000000)==0x80000000)&&CURRENT_STATE.REGS[rs]!=0){
            NEXT_STATE.PC=CURRENT_STATE.PC+offse;
            //NEXT_STATE.PC=CURRENT_STATE.PC+offset+4;
        }
        else {
            NEXT_STATE.PC=CURRENT_STATE.PC+4;
        }
		break;     

        case ADDI:
        extend_imm=(imm<<16)>>16;
        NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+extend_imm;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;    
        break;
         
        case ADDIU:
        extend_imm=(imm<<16)>>16; 
        NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]+extend_imm;
		NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SLTI:
        extend_imm=(imm<<16)>>16;
        if(((int32_t)CURRENT_STATE.REGS[rs]-(int32_t)extend_imm)<0){
            NEXT_STATE.REGS[rt]=1;
        }  
        else{
            NEXT_STATE.REGS[rt]=0; 
        }
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case SLTIU:
        extend_imm=(imm<<16)>>16;
        if((CURRENT_STATE.REGS[rs])<(uint32_t)extend_imm){
            NEXT_STATE.REGS[rt]=1;
        }  
        else{
            NEXT_STATE.REGS[rt]=0; 
        }
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case ANDI:
        u_extend_imm=(uint32_t)imm;
        NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]&u_extend_imm;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case ORI:
        u_extend_imm=(uint32_t)imm;
        NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]|u_extend_imm;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;         

        case XORI:
        u_extend_imm=(uint32_t)imm;
        NEXT_STATE.REGS[rt]=CURRENT_STATE.REGS[rs]^u_extend_imm;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;    

        case LUI:
        NEXT_STATE.REGS[rt]=(imm<<16)&0xFFFF0000;
        NEXT_STATE.PC=CURRENT_STATE.PC+4;
        break;

        case LB:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        int8_t lb_byte=(int8_t)(mem_read_32(address)&0x000000FF); 
	    NEXT_STATE.REGS[rt]=(lb_byte<<24)>>24; 
	    NEXT_STATE.PC = CURRENT_STATE.PC+4; 
        break;

        case LH:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        int16_t lh_half=(int16_t)(mem_read_32(address)&0x0000FFFF); 
	    NEXT_STATE.REGS[rt]=(lh_half<<16)>>16; 
	    NEXT_STATE.PC = CURRENT_STATE.PC+4; 
        break;

        case LW:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        int32_t lw_word=(int32_t)(mem_read_32(address)&0x0000FFFF); 
	    NEXT_STATE.REGS[rt]=lw_word; 
	    NEXT_STATE.PC = CURRENT_STATE.PC+4;  
        break;

        case LBU:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        uint8_t lbu_byte=(uint8_t)(mem_read_32(address)&0x000000FF); 
	    NEXT_STATE.REGS[rt]=(uint32_t)lbu_byte; 
	    NEXT_STATE.PC = CURRENT_STATE.PC+4;  
        break;   

        case LHU:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        uint16_t lhu_half=(uint16_t)(mem_read_32(address)&0x0000FFFF); 
	    NEXT_STATE.REGS[rt]=(uint32_t)lhu_half; 
	    NEXT_STATE.PC = CURRENT_STATE.PC+4; 
        break;

        case SB:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
	    uint32_t sb_mem_value=mem_read_32(address)&0xFFFFFF00;
	    uint32_t sb_byte=sb_mem_value|(CURRENT_STATE.REGS[rt]&0x000000FF);
	    mem_write_32(address,sb_byte);
        NEXT_STATE.PC = CURRENT_STATE.PC+4; 
        break;

        case SH:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
	    uint32_t sh_mem_value=mem_read_32(address)&0xFFFF0000;
	    uint32_t sh_half=sh_mem_value|(CURRENT_STATE.REGS[rt]&0x0000FFFF);
	    mem_write_32(address,sh_half);
        NEXT_STATE.PC = CURRENT_STATE.PC+4; 
        break;

        case SW:
        address=CURRENT_STATE.REGS[rs]+((imm<<16)>>16);
        mem_write_32(address, CURRENT_STATE.REGS[rt]);
        NEXT_STATE.PC = CURRENT_STATE.PC+4;
        break;

    }

}

void J_Type(uint32_t instruction)
{
    //首先对J型的格式进行解析
    uint8_t op=0x0;
    op=(instruction>>26)&0x3F;
    uint32_t target = 0;
    target=instruction&0x03FFFFFF;

    switch(op)
    {
        case J:
        NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|(target<<2);
        break;

        case JAL:
        NEXT_STATE.PC=(CURRENT_STATE.PC&0xF0000000)|(target<<2);
        NEXT_STATE.REGS[31]=CURRENT_STATE.PC+4;	//lab s1实验书中说因为没有实现分支延迟槽，用PC+4，实际是PC+8
		break;
    }

}