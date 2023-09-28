	.text
main:

l_1:
    addiu $v0, $zero, 10
    addiu $11, $t0, 300
	addiu $12, $zero, 500
    slt   $4, $11, $12
    slti  $5, $11,400
    mult  $11, $12
    div   $12, $11
    mflo  $13       #将除法的商存入t5
    mfhi  $14       #将除法的商存入
    bltz  $4, l_2
    bgez  $4, l_2

l_2:
    nor   $15, $11, $12
    jal   l_3

l_3:
    sllv   $16, $11, $13
    srlv  $17, $11, $13
    srav  $18, $11, $13

    syscall
