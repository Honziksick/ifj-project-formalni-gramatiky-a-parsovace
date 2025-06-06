
/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           precedence_stack.h                                        *
 * Autor:            Jan Kalina   <xkalinj00>                                  *
 *                                                                             *
 * Datum:            11.11.2024                                                *
 * Poslední změna:   03.12.2024                                                *
 *                                                                             *
 * Tým:      Tým xkalinj00                                                     *
 * Členové:  Farkašovský Lukáš    <xfarkal00>                                  *
 *           Hýža Pavel           <xhyzapa00>                                  *
 *           Kalina Jan           <xkalinj00>                                  *
 *           Krejčí David         <xkrejcd00>                                  *
 *                                                                             *
 ******************************************************************************/
/**
 * @file precedence_stack.h
 * @author Jan Kalina \<xkalinj00>
 *
 * @brief Hlavičkový soubor pro správu precedenčního zásobníku.
 * @details Tento soubor deklaruje funkce a datové struktury pro práci s
 *          precedenčním zásobníkem, který je používán při precedenční
 *          syntaktické analýze. Zásobník umožňuje manipulaci s terminály,
 *          neterminály a speciálními symboly (např. `<HANDLE>`). Obsahuje také
 *          funkce pro správu paměti, přidávání a odebírání uzlů a operace pro
 *          zjišťování vrcolu zásbníku nebo nejvrchnějšího terminálu.
 */

#ifndef PRECEDENCE_STACK_H
/** @cond  */
#define PRECEDENCE_STACK_H
/** @endcond  */

// Import submodulů parseru
#include "parser_common.h"


/*******************************************************************************
 *                                                                             *
 *                              DEFINICE KONSTANT                              *
 *                                                                             *
 ******************************************************************************/

#define SN_WITHOUT_AST_PTR  NULL                    /**<  Uzel `PrecStackNode` neobsahuje ukazatel na AST uzel.  */
#define SN_WITHOUT_AST_TYPE AST_NODE_UNDEFINED      /**<  Uzel `PrecStackNode` nemá definovaný typ AST uzlu.     */


/*******************************************************************************
 *                                                                             *
 *                             VÝČTOVÉ DATOVÉ TYPY                             *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Výčet terminálů a speciálních symbolů pro precedenční zásobník.
 *
 * @details Tento výčet definuje všechny terminály a speciální symboly, které
 *          mohou být uloženy v precedenčním zásobníku během syntaktické analýzy.
 *          Tyto symboly zahrnují identifikátory, literály, operátory a speciální
 *          symboly používané při syntaktické analýze výrazů v jazyce IFJ24.
 */
typedef enum PrecStackSymbol {
    PREC_STACK_SYM_UNEDFINED             = -1,       /**<  Typ "PrecStack" symbolu zatím není známý         */
    PREC_STACK_SYM_ID                    = 0,        /**<  Identifikátor                                    */
    PREC_STACK_SYM_INT_LITERAL           = 1,        /**<  Literál typu i32                                 */
    PREC_STACK_SYM_FLOAT_LITERAL         = 2,        /**<  Literál typu i64                                 */
    PREC_STACK_SYM_STRING_LITERAL        = 3,        /**<  Literál typu []u8                                */
    PREC_STACK_SYM_NULL_LITERAL          = 4,        /**<  Literál typu NULL                                */
    PREC_STACK_SYM_IFJ                   = 5,        /**<  Klíčové slovo "ifj"                              */
    PREC_STACK_SYM_DOT                   = 6,        /**<  Symbol tečky "."                                 */
    PREC_STACK_SYM_LEFT_BRACKET          = 7,        /**<  Symbol levé závorky "("                          */
    PREC_STACK_SYM_RIGHT_BRACKET         = 8,        /**<  Symbol pravé závorky ")"                         */
    PREC_STACK_SYM_PLUS                  = 9,        /**<  Operátor součtu "+"                              */
    PREC_STACK_SYM_MINUS                 = 10,       /**<  Operátor rozdílu "-"                             */
    PREC_STACK_SYM_MULTIPLICATION        = 11,       /**<  Operátor součinu "*"                             */
    PREC_STACK_SYM_DIVISION              = 12,       /**<  Operátor podílu "/"                              */
    PREC_STACK_SYM_IDENTITY              = 13,       /**<  Operátor rovnosti "=="                           */
    PREC_STACK_SYM_NOT_EQUAL             = 14,       /**<  Operátor nerovnosti "!="                         */
    PREC_STACK_SYM_LESS_THAN             = 15,       /**<  Operátor menší než "<"                           */
    PREC_STACK_SYM_GREATER_THAN          = 16,       /**<  Operátor větší než ">"                           */
    PREC_STACK_SYM_LESS_THAN_OR_EQUAL    = 17,       /**<  Operátor menší rovno "<="                        */
    PREC_STACK_SYM_GREATER_THAN_OR_EQUAL = 18,       /**<  Operátor větší rovno ">="                        */
    PREC_STACK_SYM_COMMA                 = 19,       /**<  Symbol čárky "," uvnitř argumentů volání funkce  */
    PREC_STACK_SYM_DOLLAR                = 20,       /**<  Řízení parsování předáno LL parseru              */
    PREC_STACK_SYM_EXPRESSION            = 21,       /**<  NEterminál pro výraz                             */
    PREC_STACK_SYM_ARG_LIST              = 22,       /**<  NEterminál pro seznam argumentů                  */
    PREC_STACK_SYM_ARG                   = 23,       /**<  NEterminál pro argument                          */
    PREC_STACK_SYM_HANDLE                = 24,       /**<  Speciální symbol "<" pro handle                  */
} PrecStackSymbol;

/**
 * @brief Výčtový typ reprezentující symboly neterminálů v precedenčním zásobníku.
 *
 * @details Tento výčtový typ definuje různé neterminály, které mohou být použity
 *          v precedenčním zásobníku během syntaktické analýzy. Každý neterminál
 *          reprezentuje specifický typ neterminálu nebo speciální symbol pro handle.
 */
typedef enum PrecStackNonTerminals {
    PREC_STACK_NT_UNEDFINED  = -1,      /**<  Typ zásobníkového neterminálu zatím není známý  */
    PREC_STACK_NT_EXPRESSION = 0,       /**<  NEterminál pro výraz                            */
    PREC_STACK_NT_ARG_LIST   = 1,       /**<  NEterminál pro seznam argumentů                 */
    PREC_STACK_NT_HANDLE     = 2,       /**<  Speciální symbol "<" pro handle                 */
} PrecStackNonTerminals;

/**
 * @brief Výčtový typ reprezentující typy uzlů v precedenčním zásobníku.
 *
 * @details Tento výčtový typ definuje různé typy uzlů, které mohou být uloženy
 *          v precedenčním zásobníku během syntaktické analýzy. Každý typ uzlu
 *          reprezentuje buď terminál, neterminál, nebo speciální symbol pro handle.
 */
typedef enum PrecStackNodeType {
    STACK_NODE_TYPE_UNDEFINED    = -1,       /**<  Typ zásobníkového uzlu zatím není známý  */
    STACK_NODE_TYPE_TERMINAL     = 1,        /**<  Uzel reprezentující terminál             */
    STACK_NODE_TYPE_NONTERMINAL  = 2,        /**<  Uzel reprezentující neterminál           */
    STACK_NODE_TYPE_HANDLE       = 3,        /**<  Speciální symbol pro handle              */
} PrecStackNodeType;


/*******************************************************************************
 *                                                                             *
 *                             DEKLARACE STRUKTUR                              *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Struktura reprezentující uzel (položku) precedenčního zásobníku.
 *
 * @details Tato struktura reprezentuje jeden uzel na precedenčním zásobníku.
 *          Každý uzel obsahuje informace o typu symbolu (terminál/neterminál/handle),
 *          samotný symbol, typ uzlu v AST, ukazatel na uzel AST spojený s tímto
 *          symbolem a ukazatel na další uzel na zásobníku. AST uzly obsahují
 *          pouze zásobníkové uzly obsahující identifikátor, literál nebo
 *          neterminál pro výraz či seznam argumentů funkce.
 *
 * @note - AST = abstraktní syntaktický strom
 */
typedef struct PrecStackNode {
    PrecStackNodeType    symbolType;       /**< Typ symbolu (terminál nebo neterminál nebo handle)          */
    PrecStackSymbol      symbol;           /**< Terminál nebo neterminál uložený v uzlu                     */
    AST_NodeType         nodeType;         /**< Typ AST uzlu                                                */
    void                 *node;            /**< Ukazatel na uzel AST spojený s terminálem nebo neterminálem */
    struct PrecStackNode *next;            /**< Ukazatel na další uzel na zásobníku                         */
} PrecStackNode;

/**
 * @brief Struktura reprezentující jeden precedenční zásobník.
 *
 * @details Tato struktura reprezentuje celý precedenční zásobník. Obsahuje
 *          ukazatel na vrchol zásobníku, což umožňuje efektivní provádění
 *          operací push, pop a top.
 */
typedef struct PrecStack {
    PrecStackNode *top;             /**< Ukazatel na vrchol zásobníku         */
    struct PrecStack *next;         /**< Ukazatel na další zásobník v seznamu */
} PrecStack;

/**
 * @brief Struktura reprezentující seznam precedenčních zásobníků.
 *
 * @details Obsahuje ukazatel na aktuální zásobník používaný během analýzy.
 *
 * @note Nejedná se ani tolik o seznam precedenčních zásobníků než spíše
 *       o zásobník všech aktuálně alokovaných precedenčních zásobníků.
 *       Pojem "seznam" jsem zvolil s cílem lépe rozlišit jednotlivé struktury
 *       a příslušné funkce.
 */
typedef struct PrecStackList {
    PrecStack *stack;               /**< Ukazatel na aktuální (nejvrchnější) zásobník v seznamu */
} PrecStackList;

/*******************************************************************************
 *                                                                             *
 *                       DEKLARACE GLOBÁLNÍCH PROMĚNNÝCH                       *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Globální proměnná pro seznam precedenčních zásobníků.
 *
 * @details Tato proměnná uchovává ukazatel na aktuální precedenční zásobník,
 *          který je používán během syntaktické analýzy. Seznam zásobníků je
 *          alokován na začátku syntaktické analýzy a uvolněn po jejím skončení.
 */
extern struct PrecStackList *precStackList;


/*******************************************************************************
 *                                                                             *
 *                         DEKLARACE VEŘEJNÝCH FUNKCÍ                          *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Inicializuje globální seznam precedenčních zásobníků.
 *
 * @details Tato funkce alokuje paměť pro globální seznam precedenčních zásobníků
 *          a inicializuje jej. Pokud se alokace nezdaří, hlásí interní chybu.
 */
void precStackList_create();

/**
 * @brief Uvolní globální strukturu seznamu zásobníků.
 *
 * @details Tato funkce uvolní paměť alokovanou pro globální seznam zásobníků.
 *          Pokud je seznam prázdný, funkce neprovádí žádnou akci.
 */
void precStackList_destroy();

/**
 * @brief Přidá nový precedenční zásobník na vrchol seznamu a inicializuje jej.
 *
 * @details Tato funkce vytvoří nový precedenční zásobník, inicializuje jej a
 *          přidá jej na vrchol globálního seznamu zásobníků. Pokud se alokace
 *          nezdaří, hlásí interní chybu.
 */
void precStackList_push();

/**
 * @brief Odebere a uvolní vrcholový zásobník ze seznamu (bez uvolnění AST uzlů).
 *
 * @details Tato funkce odebere vrcholový zásobník ze seznamu a uvolní jeho paměť,
 *          aniž by uvolnila paměť pro uzly abstraktního syntaktického stromu (AST).
 *          Pokud je seznam prázdný, funkce neprovádí žádnou akci.
 */
void precStackList_pop();

/**
 * @brief Uvolní všechny zásobníky a jejich obsah ze seznamu
 *        pomocí @c precStack_purge() (včetně uvolnění AST uzlů).
 *
 * @details Tato funkce uvolní všechny zásobníky ze seznamu pomocí funkce
 *          @c precStack_purge(). Pokud je seznam prázdný, funkce neprovádí
 *          žádnou akci.
 */
void precStackList_purge();

/**
 * @brief Vloží precedenční terminál na aktuální precedenční zásobník.
 *
 * @details Tato funkce přidá nový terminál na vrchol precedenčního zásobníku.
 *          Pokud není zásobník alokovaný, funkce neprovádí žádnou akci.
 *
 * @param [in] symbol Terminál k vložení na precedenční zásobník.
 * @param [in] type Typ AST uzlu spojeného s terminálem.
 *                  (typ `AST_NodeType' nebo `SN_WITHOUT_AST_TYPE`)
 * @param [in] node Ukazatel na AST uzel spojený s terminálem.
 *                  (ukazatel na uzel AST nebo `SN_WITHOUT_AST_PTR`)
 */
void precStack_pushPrecTerminal(PrecTerminals symbol, AST_NodeType type, void *node);

/**
 * @brief Vloží NEterminál na globální precedenční zásobník.
 *
 * @details Tato funkce přidá nový NEterminál na vrchol precedenčního zásobníku.
 *          Pokud není zásobník alokovaný, funkce neprovádí žádnou akci.
 *
 * @param [in] symbol NEterminál k vložení na precedenční zásobník.
 * @param [in] type Typ AST uzlu spojeného s NEterminálem
 *                  (typ `AST_NodeType' nebo `SN_WITHOUT_AST_TYPE`)
 * @param [in] node Ukazatel na AST uzel spojený s NEterminálem.
 *                  (ukazatel na uzel AST nebo `SN_WITHOUT_AST_PTR`)
 */
void precStack_pushPrecNonTerminal(PrecStackNonTerminals symbol, AST_NodeType type, void *node);

/**
 * @brief Vloží inicializovaný Stack uzel na zásobník, popř. i s AST uzlem.
 *
 * @details Tato funkce vytváří a inicializuje uzly AST na základě typu terminálu
 *          a pushuje je na zásobník. Pokud je terminál typu identifikátor nebo
 *          literál, vytvoří se odpovídající uzel AST a inicializuje se. Poté se
 *          uzel pushne na zásobník. Pokud je terminál jiného typu, pushne se na
 *          zásobník bez vytvoření AST uzlu.
 *
 * @param [in] inTerminal Typ terminálu, který má být vložen na zásobník.
 */
void precStack_pushBothStackAndASTNode(PrecTerminals inTerminal);

/**
 * @brief Vloží handle za první terminál na zásobníku.
 *
 * @details Tato funkce najde první terminál na zásobníku a vloží za něj handle.
 *          Pokud není zásobník alokovaný nebo neobsahuje žádný terminál, hlásí
 *          interní chybu.
 */
void precStack_pushHandleAfterFirstTerminal();

/**
 * @brief Odebere uzel AST z globálního precedenčního zásobníku.
 *
 * @details Tato funkce odstraní vrcholový element ze zásobníku a vrátí
 *          ukazatel na tento uzel. Pokud je zásobník prázdný, vrátí @c NULL.
 *
 * @return Ukazatel na odebraný uzel, nebo @c NULL pokud je zásobník prázdný.
 */
PrecStackNode *precStack_pop();

/**
 * @brief Získá ukazatel na uzel na vrcholu globálního precedenčního zásobníku
 *        bez jeho odebrání ze zásobníku.
 *
 * @details Tato funkce vrátí ukazatel na vrcholový element zásobníku bez jeho
 *          odstranění. Pokud je zásobník prázdný, hlásí interní chybu.
 *
 * @return Ukazatel na vrcholový uzel zásobníku, nebo @c NULL pokud je zásobník
 *         prázdný.
 */
PrecStackNode *precStack_top();

/**
 * @brief Uvolní všechny zdroje spojené s uzlem @c PrecStackNode.
 *
 * @details Tato funkce uvolní všechny zdroje spojené s uzlem @c PrecStackNode,
 *          včetně volání funkce @c AST_destroyNode() pro uvolnění `void *node`.
 *
 * @param [in] stackNode Ukazatel na uzel @c PrecStackNode, který má být uvolněn.
 */
void precStack_freeNode(PrecStackNode *stackNode);

/**
 * @brief Získá terminál na vrcholu zásobníku, který je nejblíže vrcholu.
 *
 * @details Tato funkce prochází zásobník od vrcholu směrem dolů, dokud nenajde
 *          první terminál. Pokud na zásobníku není žádný terminál, hlásí
 *          interní chybu.
 *
 * @param [out] topTerminal Ukazatel na proměnnou, do které bude uložen nalezený
 *              terminál.
 */
void precStack_getTopPrecTerminal(PrecTerminals *topTerminal);

/**
 * @brief Zkontroluje, zda je na vrcholu zásobníku symbol ID.
 *
 * @details Tato funkce zkontroluje, zda je na vrcholu zásobníku symbol ID.
 *
 * @return @c true, pokud je na vrcholu zásobníku symbol ID, jinak @c false.
 */
bool precStack_isIdOnTop();

/**
 * @brief Získá výsledný AST uzel výrazu z precedenčního zásobníku.
 *
 * @details Tato funkce získá výsledný AST uzel výrazu z precedenčního zásobníku.
 *          Pokud je zásobník prázdný nebo vrcholový uzel neobsahuje výraz, hlásí
 *          interní chybu.
 *
 * @param [out] result Ukazatel na proměnnou, kam bude uložen výsledný `AST_ExprNode *`.
 */
void precStack_getResult(AST_ExprNode **result);


/*******************************************************************************
 *                                                                             *
 *                         DEKLARACE INTERNÍCH FUNKCÍ                          *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Uvolní všechny uzly z globálního precedenčního zásobníku.
 *
 * @details Tato funkce uvolní veškerou paměť alokovanou pro uzly zásobníku,
 *          ale obsažené uzly AST nechává alokované.
 *
 * @param stack Ukazatel na zásobník, který má být uvolněn.
 */
void precStack_dispose(PrecStack *stack);


/**
 * @brief Uvolní všechny uzly z globálního precedenčního zásobníku pomocí
 *        funkce @c precStack_freeNode().
 *
 * @details Tato funkce uvolní všechny uzly z globálního precedenčního zásobníku
 *          pomocí funkce @c precStack_freeNode(). Uvolňuje tedy zásobníkové
 *          uzly včetně případných AST uzlů uvnitř nich.
 *
 * @param stack Ukazatel na zásobník, který má být uvolněn.
 */
void precStack_purge(PrecStack *stack);

/**
 * @brief Zkontroluje, zda je aktuální precedenční zásobník prázdný.
 *
 * @details Tato funkce kontroluje, zda je zásobník prázdný tím, že ověří, zda
 *          ukazatel na vrchol zásobníku (top) je @c NULL.
 *
 * @return @c true, pokud je zásobník prázdný, jinak @c false.
 */
bool precStack_isEmpty();

/**
 * @brief Vytvoří nový zásobníkový uzel typu typu @c PrecStackNode.
 *
 * @details Tato funkce alokuje paměť pro nový uzel typu @c PrecStackNode
 *          a inicializuje jeho členy na výchozí hodnoty. Pokud se alokace
 *          paměti nezdaří, funkce hlásí interní chybu a vrácí @c NULL.
 *
 * @return Ukazatel na nově vytvořený uzel typu @c PrecStackNode,
 *         nebo @c NULL v případě chyby alokace.
 */
PrecStackNode *precStack_createStackNode();

/**
 * @brief Namapuje typ precedenčního terminálu na typ zásobníkového symbolu.
 *
 * @details Tato funkce přijímá precedenční terminál a mapuje jej na
 *          odpovídajícízásobníkový symbol. Pokud je předán neplatný ukazatel,
 *          funkce hlásí interní chybu. Pokud je předán neterminál nebo handle,
 *          funkce zavolá @c parser_errorWatcher() a nastavuje chybu typu
 *          @c ERROR_INTERNAL.
 *
 * @param [in] terminal Precedenční terminál, který se má namapovat.
 * @param [out] stackSymbol Ukazatel na zásobníkový symbol, který bude nastaven.
 */
void precStack_mapPrecTerminalToStackSymbol(PrecTerminals terminal, PrecStackSymbol *stackSymbol);

/**
 * @brief Namapuje typ zásobníkového symbolu na typ precedenčního terminálu.
 *
 * @details Tato funkce přijímá zásobníkový symbol a mapuje jej na odpovídající
 *          precedenční terminál. Pokud je předán neplatný ukazatel, funkce
 *          hlásí interní chybu. Pokud je předán neterminál nebo handle, funkce
 *          zavolá @c parser_errorWatcher() a nastavuje chybu typu @c ERROR_INTERNAL.
 *
 * @param [in] stackSymbol Zásobníkový symbol, který se má namapovat.
 * @param [out] terminal Ukazatel na precedenční terminál, který bude nastaven.
 */
void precStack_mapStackSymbolToPrecTerminal(PrecStackSymbol stackSymbol, PrecTerminals *terminal);

/**
 * @brief Namapuje typ zásobníkového neterminálu na typ zásobníkového symbolu.
 *
 * @details Tato funkce mapuje typ zásobníkového neterminálu na odpovídající typ
 *          zásobníkového symbolu. Pokud je předán neplatný ukazatel, funkce
 *          hlásí interní chybu. zavolá @c parser_errorWatcher() a nastavuje
 *          chybu typu @c ERROR_INTERNAL
 *
 * @param [in] stackNonTerminal Zásobníkový neterminál, který má být namapován.
 * @param [out] symbol Ukazatel na zásobníkový symbol, do kterého bude uložena
 *                     namapovaná hodnota.
 */
void precStack_mapStackNonTerminalToStackSymbol(PrecStackNonTerminals stackNonTerminal, \
                                                PrecStackSymbol *symbol);

#endif // PRECEDENCE_STACK_H

/*** Konec souboru precedence_stack.h ***/
