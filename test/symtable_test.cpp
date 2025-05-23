/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           symtable_test.cpp                                         *
 * Autor:            Farkašovský Lukáš  <xfarkal00> (hlavní testy)             *
 *                   Krejčí David       <xkrejcd00> (doplnění testů)           *
 *                                                                             *
 * Datum:            06.11.2024                                                *
 * Poslední změna:   29.11.2024                                                *
 *                                                                             *
 * Tým:      Tým xkalinj00                                                     *
 * Členové:  Farkašovský Lukáš    <xfarkal00>                                  *
 *           Hýža Pavel           <xhyzapa00>                                  *
 *           Kalina Jan           <xkalinj00>                                  *
 *           Krejčí David         <xkrejcd00>                                  *
 *                                                                             *
 ******************************************************************************/
/**
 * @file symtable_test.cpp
 * @author Farkašovský Lukáš  \<xfarkal00>
 *
 * @brief Testy funkcí knihovny s implementací tabulky symbolů
 * @details Tento zdrojový soubor obsahuje unit testy funkcí implementovaných
 *          v souboru `symtable.c`. Testy jsou vytvořené za využití frameworku
 *          Google Test.
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ifj24_compiler_test_utils.h"

/**
 * @brief Testuje funkci `symtable_destroyTable` pro zničení tabulky
 */
TEST(NoTable, HashFunction) {
    DString *str = DString_init();
    ASSERT_NE(str, nullptr);

    // String "abc"
    ASSERT_EQ(DString_appendChar(str, 'a'), STRING_SUCCESS);
    ASSERT_EQ(DString_appendChar(str, 'b'), STRING_SUCCESS);
    ASSERT_EQ(DString_appendChar(str, 'c'), STRING_SUCCESS);

    size_t hash = symtable_hashFunction(str);
    // Po dosazení do hashovací funkce hash = ((hash << 5) + hash) + (size_t)key->str[i];
    size_t result = 193485963;

    EXPECT_EQ(hash, result);

    DString_free(str);
}

/**
 * @brief Testuje funkci `symtable_deleteAll` pro odstranění všech položek tabulky, která je prázdná
 */
TEST(NoTable, SymDelAll) {
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    symtable_deleteAll(map, true);

    free(map->array);
    free(map);
}

/**
 * @brief Testuje funkci `symtable_destroyTable` pro zničení tabulky
 */
TEST(NoTable, DestroyTable) {
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    symtable_destroyTable(map);
    symtable_destroyTable(NULL);
}

/**
 * @brief Testuje funkci `symtable_initItems` pro inicializaci položek tabulky
 */
TEST(NoTable, SymInitItem) {
    SymtableItemPtr item = symtable_initItems(10);

    ASSERT_NE(item, nullptr);
    ASSERT_EQ(item[0].key, nullptr);
    ASSERT_EQ(item[0].symbolState, SYMTABLE_SYMBOL_EMPTY);

    free(item);
}

/**
 * @brief Testuje funkci `symtable_init` pro inicializaci tabulky
 */
TEST(NoTable, SymInit)
{
    // Vytvoření hashovací tabulky
    SymtablePtr map = symtable_init();

    // TABLE_INIT_SIZE 10, je prázdná, tak by neměla nic používat USED_SIZE = 0
    size_t SIZE = 10;
    size_t USED_SIZE = 0;

    // Ověření, že nebyl vrácen ukazatel NULL, resp. podařilo se alokovat paměť
    ASSERT_NE(map, nullptr);

    // Ověření správné inicializace všech prvků struktury "hash_map_t"
    EXPECT_NE(map->array, nullptr);
    EXPECT_EQ(map->allocatedSize, SIZE);
    EXPECT_EQ(map->usedSize, USED_SIZE);

    // Uvolnění celé prázdné hashovací tabulky z paměti
    symtable_destroyTable(map);
}

/**
 * @brief Testuje funkci `symtable_addItem` pro přidání položek do tabulky
 */
TEST(Table, SymAddItems)
{
    // Vložení první položky a kontrola stavu tabulky po jejím vložení
    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);
    DString* horseVal1 = DString_init();
    ASSERT_NE(horseVal1, nullptr);
    DString* horseVal2 = DString_init();
    ASSERT_NE(horseVal2, nullptr);
    DString* horseVal3 = DString_init();
    ASSERT_NE(horseVal3, nullptr);
    DString* horseVal4 = DString_init();
    ASSERT_NE(horseVal4, nullptr);
    DString* horseVal5 = DString_init();
    ASSERT_NE(horseVal5, nullptr);
    DString* horseVal6 = DString_init();
    ASSERT_NE(horseVal6, nullptr);
    DString* horseVal7 = DString_init();
    ASSERT_NE(horseVal7, nullptr);
    DString* horseVal8 = DString_init();
    ASSERT_NE(horseVal8, nullptr);

    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);


    // Konstanty počtu položek a kapicity tohoto prostředí
    size_t SIZE = 10;
    size_t USED_SIZE = 0;

    // Konstanty pro testování stringů
    const char *strConst  = "horse";
    const char *strConst1 = "horse1";
    const char *strConst2 = "horse2";
    const char *strConst3 = "horse3";
    const char *strConst4 = "horse4";
    const char *strConst5 = "horse5";
    const char *strConst6 = "horse6";
    const char *strConst7 = "horse7";
    const char *strConst8 = "horse8";

    // Vložení slova do horseVal
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }
    // Vložení slova do horseVal1 - horseVal7
    for(size_t i = 0; i < strlen(strConst1); i++) {
        ASSERT_EQ(DString_appendChar(horseVal1, strConst1[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal2, strConst2[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal3, strConst3[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal4, strConst4[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal5, strConst5[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal6, strConst6[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal7, strConst7[i]), STRING_SUCCESS);
        ASSERT_EQ(DString_appendChar(horseVal8, strConst8[i]), STRING_SUCCESS);
    }
    

    size_t index = symtable_hashFunction(horseVal) % map->allocatedSize;

    EXPECT_EQ(map->usedSize, USED_SIZE);
    EXPECT_EQ(map->allocatedSize, SIZE);

    // Pokud je klíč NULL nebo tabulka NULL, vracíme chybu
    EXPECT_EQ(symtable_addItem(map, NULL, NULL), SYMTABLE_KEY_NULL);
    EXPECT_EQ(symtable_addItem(NULL, horseVal, NULL), SYMTABLE_TABLE_NULL);
    EXPECT_EQ(map->usedSize, USED_SIZE);

    // Přidání itemu do tabulky
    ASSERT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);

    EXPECT_EQ(map->usedSize, USED_SIZE + 1);
    EXPECT_EQ(map->allocatedSize, SIZE);
    EXPECT_EQ(DString_compare(map->array[index].key, horseVal), STRING_EQUAL);

    index = symtable_hashFunction(horseVal1) % map->allocatedSize;
    symtable_addItem(map, horseVal1, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 2);
    EXPECT_EQ(map->allocatedSize, SIZE);
    EXPECT_EQ(DString_compare(map->array[index].key, horseVal1), STRING_EQUAL);
    

    symtable_addItem(map, horseVal2, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 3);
    EXPECT_EQ(map->allocatedSize, SIZE);

    symtable_addItem(map, horseVal3, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 4);
    EXPECT_EQ(map->allocatedSize, SIZE);

    symtable_addItem(map, horseVal4, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 5);
    EXPECT_EQ(map->allocatedSize, SIZE);

    ASSERT_EQ(symtable_addItem(map, horseVal5, NULL), SYMTABLE_SUCCESS);

    EXPECT_EQ(map->usedSize, USED_SIZE + 6);
    EXPECT_EQ(map->allocatedSize, SIZE);

    symtable_addItem(map, horseVal6, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 7);
    EXPECT_EQ(map->allocatedSize, SIZE);

    symtable_addItem(map, horseVal7, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 8);
    EXPECT_EQ(map->allocatedSize, SIZE*2);

    symtable_addItem(map, horseVal8, NULL);

    EXPECT_EQ(map->usedSize, USED_SIZE + 9);
    EXPECT_EQ(map->allocatedSize, SIZE*2);

    symtable_destroyTable(map);
    DString_free(horseVal);
    DString_free(horseVal1);
    DString_free(horseVal2);
    DString_free(horseVal3);
    DString_free(horseVal4);
    DString_free(horseVal5);
    DString_free(horseVal6);
    DString_free(horseVal7);
    DString_free(horseVal8);
}

/**
 * @brief Testuje funkci `symtable_deleteItem` pro odstranění položky z tabulky
 */
TEST(Table, SymRemItem) {

    // Vložení první položky a kontrola stavu tabulky po jejím vložení
    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);

    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    // Konstanty pro testování stringů
    const char *strConst  = "horse";

     for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }

    // Přidání itemu do tabulky
    ASSERT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);

    // Odstranění položky
    EXPECT_EQ(symtable_deleteItem(map, horseVal), SYMTABLE_SUCCESS);
    EXPECT_EQ(symtable_deleteItem(NULL, horseVal), SYMTABLE_TABLE_NULL);
    EXPECT_EQ(symtable_deleteItem(map, NULL), SYMTABLE_KEY_NULL);

    symtable_destroyTable(map);
    DString_free(horseVal);
}

/**
 * @brief Testuje funkci `symtable_findItem` pro nalezení položky v tabulce
 */
TEST(Table, SymFindItem) {
    
        // Vložení první položky a kontrola stavu tabulky po jejím vložení
        DString* horseVal = DString_init();
        ASSERT_NE(horseVal, nullptr);
    
        SymtablePtr map = symtable_init();
        ASSERT_NE(map, nullptr);
    
        // Konstanty pro testování stringů
        const char *strConst  = "horse";
    
        for(size_t i = 0; i < strlen(strConst); i++) {
            ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
        }
    
        // Přidání itemu do tabulky
        ASSERT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);
    
        // Odstranění položky
        EXPECT_EQ(symtable_findItem(map, horseVal, NULL), SYMTABLE_SUCCESS);
        EXPECT_EQ(symtable_findItem(NULL, horseVal, NULL), SYMTABLE_TABLE_NULL);
        EXPECT_EQ(symtable_findItem(map, NULL, NULL), SYMTABLE_KEY_NULL);
    
        symtable_destroyTable(map);
        DString_free(horseVal);
}

/**
 * @brief Testuje funkci `symtable_deleteAll` pro odstranění všech položek tabulky
 */
TEST(Table, SymDelALL) {
        
    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);
        
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);
        
    SymtableItemPtr item;
        
    // Konstanty pro testování stringů
     const char *strConst  = "horse";
        
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
     }
        
     // Přidání itemu do tabulky
     ASSERT_EQ(symtable_addItem(map, horseVal, &item), SYMTABLE_SUCCESS);
        
     // Odstranění položky
     symtable_deleteAll(map, true);

     // Free bordelu, co jsem s i v testu vytvořil
     free(map->array);
     free(map);
     DString_free(horseVal);
}

/**
 * @brief Testuje funkci `symtable_addToken` pro přidání položky z tokenu do tabulky
 */
TEST(Table, SymAddToken) {

    Token* token = (Token*)malloc(sizeof(Token));
    ASSERT_NE(token, nullptr);

    DString* horseVal = DString_init();

    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    // Konstanty pro testování stringů
    const char *strConst  = "horse";

    token->type = TOKEN_IDENTIFIER;
    token->value = horseVal;
    ASSERT_NE(token->value, nullptr);

    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(token->value, strConst[i]), STRING_SUCCESS);
    }

    // Přidání itemu do tabulky
    ASSERT_EQ(symtable_addItem(map, token->value, NULL), SYMTABLE_SUCCESS);

    // Odstranění položky
    symtable_destroyTable(map);
    DString_free(token->value);
    free(token);
}

/**
 * @brief Testuje funkci `symtable_transfer` pro přenesení položek z jedné tabulky do druhé
 */
TEST(Table, SymTransfer) {
    
    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);
    
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    SymtablePtr map2 = symtable_init();
    ASSERT_NE(map2, nullptr);

    // Konstanty pro testování stringů
    const char *strConst  = "horse";
    
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }
    
    // Přidání itemu do tabulky
    ASSERT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);
    
    // Přeneseme data z jedné položky do druhé
    EXPECT_TRUE(symtable_transfer(map, map2));

    EXPECT_FALSE(symtable_transfer(NULL, map2));
    EXPECT_FALSE(symtable_transfer(NULL, map));
    EXPECT_FALSE(symtable_transfer(map, NULL));
    EXPECT_FALSE(symtable_transfer(map2, NULL));

    EXPECT_EQ(symtable_findItem(map2, horseVal, NULL), SYMTABLE_SUCCESS);
    
    symtable_destroyTable(map);
    symtable_destroyTable(map2);
    DString_free(horseVal);
}

/**
 * @brief Testuje funkci `symtable_transfer` pro přenesení položek z jedné prázdné tabulky do druhé
 */
TEST(Table, SymTransferEmpty) {
    
    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);
    
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    SymtablePtr map2 = symtable_init();
    ASSERT_NE(map2, nullptr);

    // Konstanty pro testování stringů
    const char *strConst  = "horse";
    
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }
    
    // Přidání itemu do tabulky
    ASSERT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);
    
    // Přeneseme data z jedné položky do druhé
    EXPECT_TRUE(symtable_transfer(map2, map));
    
    symtable_destroyTable(map);
    symtable_destroyTable(map2);
    DString_free(horseVal);
}

/**
 * @brief Testuje funkci `symtable_resize` pro zvětšení tabulky
 */
TEST(Table, SymResize) {

    size_t SIZE = 10;

    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);
    
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    SymtablePtr map2 = symtable_init();
    ASSERT_NE(map2, nullptr);
    
    // Konstanty pro testování stringů
    const char *strConst  = "horse";
    
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }
    
    // Přidání itemu do tabulky
    EXPECT_EQ(symtable_addItem(map, horseVal, NULL), SYMTABLE_SUCCESS);
    EXPECT_EQ(map->allocatedSize, SIZE);
    
    map2 = symtable_resize(map2, map2->allocatedSize*2);

    EXPECT_EQ(map2->allocatedSize, SIZE*2);

    symtable_destroyTable(map);
    symtable_destroyTable(map2);
    DString_free(horseVal);
}

/**
 * @brief Testuje práci s daty proměnné v tabulce symbolů
 */
TEST(Table, ItemData) {
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);

    // Konstanty pro testování stringů
    const char *strConst  = "horseInt";
    
    for(size_t i = 0; i < strlen(strConst); i++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[i]), STRING_SUCCESS);
    }


    SymtableItemPtr item1;
    ASSERT_EQ(symtable_addItem(map, horseVal, &item1), SYMTABLE_SUCCESS);
    ASSERT_NE(item1, nullptr);
    ASSERT_EQ(DString_compare(item1->key, horseVal), STRING_EQUAL);

    item1->constant = true;
    item1->knownValue = true;
    item1->used = false;
    item1->symbolState = SYMTABLE_SYMBOL_VARIABLE_INT;
    item1->data = malloc(sizeof(int));
    ASSERT_NE(item1->data, nullptr);
    *((int*)item1->data) = 5;

    SymtableItemPtr item2;
    ASSERT_EQ(symtable_findItem(map, horseVal, &item2), SYMTABLE_SUCCESS);
    ASSERT_EQ(item1->constant, item2->constant);
    ASSERT_EQ(item1->knownValue, item2->knownValue);
    ASSERT_EQ(item1->used, item2->used);
    ASSERT_EQ(item1->symbolState, item2->symbolState);
    ASSERT_EQ(*((int*)item2->data), 5);

    free(item1->data);
    symtable_destroyTable(map);
    DString_free(horseVal);
}

/**
 * @brief Testuje práci s daty funkce v tabulce symbolů
 */
TEST(Table, FunctionData) {
    SymtablePtr map = symtable_init();
    ASSERT_NE(map, nullptr);

    DString* horseVal = DString_init();
    ASSERT_NE(horseVal, nullptr);

    DString* a = DString_init();
    ASSERT_NE(a, nullptr);
    ASSERT_EQ(DString_appendChar(a, 'a'), STRING_SUCCESS);
    DString* b = DString_init();
    ASSERT_NE(b, nullptr);
    ASSERT_EQ(DString_appendChar(b, 'b'), STRING_SUCCESS);
    DString* c = DString_init();
    ASSERT_NE(c, nullptr);
    ASSERT_EQ(DString_appendChar(c, 'c'), STRING_SUCCESS);
    DString* d = DString_init();
    ASSERT_NE(d, nullptr);
    ASSERT_EQ(DString_appendChar(d, 'd'), STRING_SUCCESS);
    DString* e = DString_init();
    ASSERT_NE(e, nullptr);
    ASSERT_EQ(DString_appendChar(e, 'e'), STRING_SUCCESS);
    DString* f = DString_init();
    ASSERT_NE(f, nullptr);
    ASSERT_EQ(DString_appendChar(f, 'f'), STRING_SUCCESS);
    DString* g = DString_init();
    ASSERT_NE(g, nullptr);
    ASSERT_EQ(DString_appendChar(g, 'g'), STRING_SUCCESS);
    DString* h = DString_init();
    ASSERT_NE(h, nullptr);
    ASSERT_EQ(DString_appendChar(h, 'h'), STRING_SUCCESS);
    DString* i = DString_init();
    ASSERT_NE(i, nullptr);
    ASSERT_EQ(DString_appendChar(i, 'i'), STRING_SUCCESS);


    // Konstanty pro testování stringů
    const char *strConst  = "morseFn";
    
    for(size_t k = 0; k < strlen(strConst); k++) {
        ASSERT_EQ(DString_appendChar(horseVal, strConst[k]), STRING_SUCCESS);
    }

    // Přidání položky do tabulky
    SymtableItemPtr item1;
    ASSERT_EQ(symtable_addItem(map, horseVal, &item1), SYMTABLE_SUCCESS);

    // Inicializace dat funkce
    SymtableFunctionData *data = symtable_initFunctionData(2);
    ASSERT_NE(data, nullptr);
    ASSERT_EQ(data->paramCount, 2ULL);

    data->returnType = SYMTABLE_TYPE_VOID;

    // Nastavíme data 1. parametru
    data->params[0].id = DString_init();
    ASSERT_NE(data->params[0].id, nullptr);
    ASSERT_EQ(DString_appendChar(data->params[0].id, 'p'), STRING_SUCCESS);
    data->params[0].type = SYMTABLE_TYPE_INT;

    // Nastavíme data 2. parametru
    data->params[1].id = DString_init();
    ASSERT_NE(data->params[1].id, nullptr);
    ASSERT_EQ(DString_appendChar(data->params[1].id, 'l'), STRING_SUCCESS);
    data->params[1].type = SYMTABLE_TYPE_STRING_OR_NULL;

    // Přiřazení dat funkce do položky
    item1->data = data;
    item1->knownValue = true;
    item1->symbolState = SYMTABLE_SYMBOL_FUNCTION;
    item1->used = false;

    // Přidáme další položky do tabulky
    ASSERT_EQ(symtable_addItem(map, a, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, b, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, c, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, d, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, e, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, f, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, g, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, h, NULL), SYMTABLE_SUCCESS);
    ASSERT_EQ(symtable_addItem(map, i, NULL), SYMTABLE_SUCCESS);
    
    // Pokusíme se najít položku
    ASSERT_EQ(symtable_findItem(map, horseVal, &item1), SYMTABLE_SUCCESS);
    SymtableFunctionData *foundData = (SymtableFunctionData *)item1->data;
    // Porovnáme data
    ASSERT_NE(foundData, nullptr);
    ASSERT_EQ(foundData->paramCount, 2ULL);
    ASSERT_EQ(foundData->returnType, SYMTABLE_TYPE_VOID);
    ASSERT_EQ(DString_compareWithConstChar(foundData->params[0].id, "p"), STRING_EQUAL);
    ASSERT_EQ(foundData->params[0].type, SYMTABLE_TYPE_INT);
    ASSERT_EQ(DString_compareWithConstChar(foundData->params[1].id, "l"), STRING_EQUAL);
    ASSERT_EQ(foundData->params[1].type, SYMTABLE_TYPE_STRING_OR_NULL);

    // Uvolňuje se v rámci destrukce AST
    DString_free(data->params[0].id);
    DString_free(data->params[1].id);

    // Uvolňuje se v rámci destrukce Symtable
    symtable_destroyTable(map);
    DString_free(horseVal);
    DString_free(a);
    DString_free(b);
    DString_free(c);
    DString_free(d);
    DString_free(e);
    DString_free(f);
    DString_free(g);
    DString_free(h);
    DString_free(i);
}