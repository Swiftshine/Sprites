.include "macros.S"
//  TODO: 
//- butterfly
//- grass
//- flower stems
//- normal maps
//- possibly morse than three flowers per-set? requires rewrite and more re

.rodata
//. Flower Data set by the setter sprite.
    flowerType: .byte 0x0
    flower1Data: .byte 0x0
    flower2Data: .byte 0x0
    flower3Data: .byte 0x0
    butterflyData: .byte 0x0
    grassData: .byte 0x0
//  Flower texture names
//. Additional custom entries, starting with 3 or 5, all the way to 15, will go here.
//. Sky Textures
    SkyFlower1: .strz "obj_hana_kogen01\0", "obj_hana_kogen02\0", "obj_hana_kogen03\0", "obj_hana_kogen04\0", "obj_hana_kogen05\0", "obj_hana_kogen06\0", "obj_hana_kogen07\0", "obj_hana_kogen08\0", "obj_hana_kogen09\0"
    SkyFlower2: .strz "obj_hana_kogen02\0", "obj_hana_kogen01\0", "obj_hana_kogen03\0", "obj_hana_kogen04\0", "obj_hana_kogen05\0", "obj_hana_kogen06\0", "obj_hana_kogen07\0", "obj_hana_kogen08\0", "obj_hana_kogen09\0"
    SkyFlower3: .strz "obj_hana_kogen03\0", "obj_hana_kogen01\0", "obj_hana_kogen02\0", "obj_hana_kogen04\0", "obj_hana_kogen05\0", "obj_hana_kogen06\0", "obj_hana_kogen07\0", "obj_hana_kogen08\0", "obj_hana_kogen09\0"

//. Standard Textures
    StandardFlower1: .strz "obj_hana01\0", "obj_hana02\0", "obj_hana03\0"
    StandardFlower2: .strz "obj_hana02\0", "obj_hana01\0", "obj_hana03\0"
    StandardFlower3: .strz "obj_hana03\0", "obj_hana01\0", "obj_hana02\0"
    StandardGrass: .strz "obj_kusa01\0", "obj_kusa02\0", "obj_kusa03\0", "obj_kusa04\0", "obj_kusa05\0", "obj_kusa06\0", "obj_kusa07\0", "obj_kusa08\0", "obj_kusa09\0", "obj_kusa10\0"
.text


.global StoreFlowerGrassInfo
StoreFlowerGrassInfo:
    // r3 = this

    stb r0, 0x3f8C(r12) // original instruction

    // Save Flower Type
    // Save r3
    subi sp, sp, 0x4
    stw r3, 0x0(sp)

    // r3 = &flowerType
    lis r3, flowerType@ha
    ori r3, r3, flowerType@l

    // flowerType = r0
    stb r0, 0x0(r3)

    // Restore r3
    lwz r3, 0x0(sp)
    addi sp, sp, 0x4


    // Save r4-r6
    subi sp, sp, 0xC
    stw r4, 0x0(sp)
    stw r5, 0x4(sp)
    stw r6, 0x8(sp)

    // r4 = &flower1Data
    lis r4, flower1Data@ha
    ori r4, r4, flower1Data@l

    // r5 = this->settings1
    lwz r5, 0x10(r3)

    // r6 = nybble 5
    srawi r6, r5, 0x1C
    andi. r6, r6, 0xF

    // flower1Data = nybble 5
    stb r6, 0x0(r4)

    // r4 = &flower2Data
    lis r4, flower2Data@ha
    ori r4, r4, flower2Data@l

    // r6 = nybble 6
    srawi r6, r5, 0x18
    andi. r6, r6, 0xF

    // flower2Data = nybble 6
    stb r6, 0x0(r4)

    // r4 = &flower3Data
    lis r4, flower3Data@ha
    ori r4, r4, flower3Data@l

    // r6 = nybble 7
    srawi r6, r5, 0x14
    andi. r6, r6, 0xF

    // flower3Data = nybble 7
    stb r6, 0x0(r4)

    // Store butterfly type
    // r4 = &butterflyData
    lis r4, butterflyData@ha
    ori r4, r4, butterflyData@l

    // r6 = nybble 8
    srawi r6, r5, 0x10
    
    // butterflyData = nybble 8
    stb r6, 0x0(r4)

    // Store grass type
    // r4 = &grassData
    lis r4, grassData@ha
    ori r4, r4, grassData@l

    // r6 = nybble 9
    srawi r6, r5, 0xC
    andi. r6, r6, 0xF

    // grassData = nybble 9
    stb r6, 0x0(r4)


    // Restore r4-r6
    lwz r4, 0x0(sp)
    lwz r5, 0x4(sp)
    lwz r6, 0x8(sp)
    addi sp, sp, 0xC

    blr

    // thanks copilot, that took a while


//. Sky Textures

.global SkyFlower1Texture
SkyFlower1Texture:

    // Save r3 and r4
    stwu sp, -0x8(sp)
    stw r3, 0x0(sp)
    stw r4, 0x4(sp)

    // r3 = flower1Data
    lis r3, flower1Data@ha
    ori r3, r3, flower1Data@l
    lbz r3, 0x0(r3)

    // r0 = &SkyFlower1 array, aka index 0
    lis r0, SkyFlower1@h
    ori r0, r0, SkyFlower1@l

    // r4 = the value of flower1Data * 17 (length of each string + null terminator)
    mulli r4, r3, 0x11

    // offset r0 by the value in r4
    add r0, r0, r4

    // Restore r3 and r4
    lwz r3, 0x0(sp)
    lwz r4, 0x4(sp)
    addi sp, sp, 0x8

    // set the texture reference
    stw r0, 0x6C(r1)

    blr

.global SkyFlower2Texture
SkyFlower2Texture:

    // Save r3 and r4
    stwu sp, -0x8(sp)
    stw r3, 0x0(sp)
    stw r4, 0x4(sp)

    // r3 = flower2Data
    lis r3, flower2Data@ha
    ori r3, r3, flower2Data@l
    lbz r3, 0x0(r3)

    // r5 = &SkyFlower2 array, aka index 0
    lis r5, SkyFlower2@h
    ori r5, r5, SkyFlower2@l

    // r4 = the value of flower2Data * 17 (length of each string + null terminator)
    mulli r4, r3, 0x11

    // offset r5 by the value in r4
    add r5, r5, r4

    // Restore r3 and r4
    lwz r3, 0x0(sp)
    lwz r4, 0x4(sp)
    addi sp, sp, 0x8

    // set the texture reference
    stw r5, 0x6C(r1)

    blr

.global SkyFlower3Texture
SkyFlower3Texture:

    // Save r3 and r4
    stwu sp, -0x8(sp)
    stw r3, 0x0(sp)
    stw r4, 0x4(sp)

    // r3 = flower3Data
    lis r3, flower3Data@ha
    ori r3, r3, flower3Data@l
    lbz r3, 0x0(r3)

    // r7 = &SkyFlower3 array, aka index 0
    lis r7, SkyFlower3@h
    ori r7, r7, SkyFlower3@l

    // r4 = the value of flower3Data * 17 (length of each string + null terminator)
    mulli r4, r3, 0x11

    // offset r7 by the value in r4
    add r7, r7, r4

    // Restore r3 and r4
    lwz r3, 0x0(sp)
    lwz r4, 0x4(sp)
    addi sp, sp, 0x8

    // set the texture reference
    stw r7, 0x6C(r1)

    blr

//. Forest Textures
.global ForestFlower1Texture
ForestFlower1Texture:

    blr

//. Standard Grass Textures
.global StandardGrassTexture1
StandardGrassTexture1:

    // Save r3 and r4
    stwu sp, -0x8(sp)
    stw r3, 0x0(sp)
    stw r4, 0x4(sp)

    // r3 = &grassData
    lis r3, grassData@ha
    ori r3, r3, grassData@l

    // r10 = &StandardGrass array, aka index 0
    lis r10, StandardGrass@h
    ori r10, r10, StandardGrass@l

    // r4 = the value of grassData * 11 (length of each string + null terminator * 5 (because we want to skip the textures that aren't grass 1))
    mulli r4, r3, 0xB
    mulli r4, r4, 0x4

    // offset r10 by the value in r4
    // add r10, r10, r4

    // Restore r3 and r4
    lwz r3, 0x0(sp)
    lwz r4, 0x4(sp)
    addi sp, sp, 0x8

    // set the texture reference
    stw r10, 0x1C(r1)

    blr
