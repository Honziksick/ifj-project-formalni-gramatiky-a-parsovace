/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           ast_interface.c                                           *
 * Autor:            Jan Kalina   <xkalinj00>                                  *
 *                                                                             *
 * Datum:            01.11.2024                                                *
 * Poslední změna:   01.12.2024                                                *
 *                                                                             *
 * Tým:      Tým xkalinj00                                                     *
 * Členové:  Farkašovský Lukáš    <xfarkal00>                                  *
 *           Hýža Pavel           <xhyzapa00>                                  *
 *           Kalina Jan           <xkalinj00>                                  *
 *           Krejčí David         <xkrejcd00>                                  *
 *                                                                             *
 ******************************************************************************/
/**
 * @file ast_interface.c
 * @author Jan Kalina \<xkalinj00>
 *
 * @brief Implementace funkcí pro práci s abstraktním syntaktickým stromem (AST).
 * @details Tento soubor obsahuje definice funkcí pro vytváření a rušení uzlů
 *          abstraktního syntaktického stromu (AST) pro překladač IFJ24. Každá
 *          funkce zajišťuje správnou inicializaci nebo uvolnění paměti pro
 *          příslušný typ uzlu AST, což usnadňuje práci s datovými strukturami
 *          v rámci sémantické analýzy a generování kódu. Pro zjednodušení práce
 *          s uzly je navrženo také veřejné rozhraní s obecnou funkcí pro tvorbu
 *          a obecnou funkcí pro destrukci uzlů. Obsaženy jsou také funkce pro
 *          inicializaci nových uzlů konkrétních typů a pro destrukci lineárních
 *          seznamů uzlů.
 */

// Import knihoven pro abstraktní syntaktický strom (AST)
#include "ast_nodes.h"
#include "ast_interface.h"

// Import knihoven pro syntaktický analyzátor
#include "parser_common.h"


/*******************************************************************************
 *                                                                             *
 *                        IMPLEMENTACE VEŘEJNÝCH FUNKCÍ                        *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Vytvoří nový uzel AST daného typu.
 */
void *AST_createNode(AST_NodeType type) {
    // Na základě typu uzlu vybereme specializovanou funkci na alokaci nových uzlů
    switch(type) {
        // Uzel pro program
        case AST_PROGRAM_NODE:
            return AST_createProgramNode();

        // Uzel pro definici funkce
        case AST_FUN_DEF_NODE:
            return AST_createFunDefNode();

        // Uzel pro argument/parametr funkce
        case AST_ARG_OR_PARAM_NODE:
            return AST_createArgOrParamNode();

        // Uzel pro příkaz
        case AST_STATEMENT_NODE:
            return AST_createStatementNode();

        // Uzel pro volání funkce
        case AST_FUN_CALL_NODE:
            return AST_createFunCallNode();

        // Uzel pro podmíněný příkaz "if"
        case AST_IF_NODE:
            return AST_createIfNode();

        // Uzel pro cyklus "while"
        case AST_WHILE_NODE:
            return AST_createWhileNode();

        // Uzel pro výraz
        case AST_EXPR_NODE:
            return AST_createExprNode();

        // Uzel pro binární operaci
        case AST_BIN_OP_NODE:
            return AST_createBinOpNode();

        // Uzel pro proměnnou
        case AST_VAR_NODE:
            return AST_createVarNode(AST_VAR_NODE);

        // Uzel pro literál
        case AST_LITERAL_NODE:
            return AST_createVarNode(AST_LITERAL_NODE);

        // Jinak vracíme NULL
        default:
            return NULL;
    } // switch()
} // AST_createNode()

/**
 * @brief Uvolní paměť uzlu AST daného typu.
 */
void AST_destroyNode(AST_NodeType type, void *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Na základě typu uzlu vybereme specializovanou funkci na destrukci uzlů
    switch(type) {
        // Uzel pro program
        case AST_PROGRAM_NODE:
            AST_destroyProgramNode((AST_ProgramNode *)node);
            break;

        // Uzel pro definici funkce
        case AST_FUN_DEF_NODE:
            AST_destroyFunDefNode((AST_FunDefNode *)node);
            break;

        // Uzel pro argument/parametr funkce
        case AST_ARG_OR_PARAM_NODE:
            AST_destroyArgOrParamNode((AST_ArgOrParamNode *)node);
            break;

        // Uzel pro příkaz
        case AST_STATEMENT_NODE:
            AST_destroyStatementNode((AST_StatementNode *)node);
            break;

        // Uzel pro volání funkce
        case AST_FUN_CALL_NODE:
            AST_destroyFunCallNode((AST_FunCallNode *)node);
            break;

        // Uzel pro podmíněný příkaz "if"
        case AST_IF_NODE:
            AST_destroyIfNode((AST_IfNode *)node);
            break;

        // Uzel pro cyklus "while"
        case AST_WHILE_NODE:
            AST_destroyWhileNode((AST_WhileNode *)node);
            break;

        // Uzel pro výraz
        case AST_EXPR_NODE:
            AST_destroyExprNode((AST_ExprNode *)node);
            break;

        // Uzel pro binární operaci
        case AST_BIN_OP_NODE:
            AST_destroyBinOpNode((AST_BinOpNode *)node);
            break;

        // Uzel pro proměnnou/literál
        case AST_VAR_NODE:
        case AST_LITERAL_NODE:
            AST_destroyVarNode((AST_VarNode *)node);
            break;

        // Jinak se nic nestane
        default:
            return;
    } // switch()
} // AST_destroyNode()

/**
 * @brief Alokuje paměť pro globální kořen abstraktního syntaktického stromu (AST).
 */
inline void AST_initTree() {
    // Zavoláme funkci pro alokaci uzlu typu AST_ProgramNode
    ASTroot = (AST_ProgramNode *)AST_createNode(AST_PROGRAM_NODE);

    // Pokud se alokace paměti nezdařila, došlo k interná chybě
    if(ASTroot == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
    }
} // AST_createTree()

/**
 * @brief Uvolní z paměti celý abstraktní syntaktický strom.
 */
void AST_destroyTree() {
    // Pokud je kořen platný, zahájí postupnou destrukci stromu
    if(ASTroot != NULL) {
        AST_destroyProgramNode(ASTroot);
        ASTroot = NULL;
    }
} // AST_destroyTree()


/*******************************************************************************
 *                                                                             *
 *        IMPLEMENTACE VEŘEJNÝCH FUNKCÍ NA KONKRÉTNÍ INICIALIZACI UZLŮ         *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Inicializuje uzel @c AST_FunDefNode pro definici funkce.
 */
void AST_initNewFunDefNode(AST_FunDefNode *node, DString *identifier, \
                           AST_ArgOrParamNode *parameters, AST_DataType returnType,  \
                           AST_StatementNode *body) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->identifier != NULL || node->parameters != NULL ||
       node->returnType != AST_DATA_TYPE_NOT_DEFINED ||
       node->body != NULL || node->next != NULL)
    {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->identifier = identifier;
    node->parameters = parameters;
    node->returnType = returnType;
    node->body = body;
    node->next = NULL;
} // AST_initNewFunDefNode()

/**
 * @brief Inicializuje uzel @c AST_ArgOrParamNode pro argument/parametr funkce.
 */
void AST_initNewArgOrParamNode(AST_ArgOrParamNode *node, AST_DataType dataType, \
                               AST_ExprNode *expression) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->expression != NULL || node->next != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->dataType = dataType;
    node->expression = expression;
    node->next = NULL;
} // AST_initNewArgOrParamNode()

/**
 * @brief Inicializuje uzel @c AST_StatementNode pro příkaz.
 */
void AST_initNewStatementNode(AST_StatementNode *node, size_t frameID, \
                              AST_StatementType statementType, void *statement) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->statement != NULL || node->next != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->frameID = frameID;
    node->statementType = statementType;
    node->statement = statement;
    node->next = NULL;
} // AST_initNewStatementNode()


/**
 * @brief Inicializuje uzel @c AST_FunCallNode pro volání funkce.
 */
void AST_initNewFunCallNode(AST_FunCallNode *node, DString *identifier, \
                            bool isBuiltIn, AST_ArgOrParamNode *arguments) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->identifier != NULL || node->arguments != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->identifier = identifier;
    node->isBuiltIn = isBuiltIn;
    node->arguments = arguments;
} // AST_initNewFunCallNode()

/**
 * @brief Inicializuje uzel @c AST_IfNode pro podmíněný příkaz if.
 */
void AST_initNewIfNode(AST_IfNode *node, AST_ExprNode *condition, AST_VarNode *nullCondition, \
                    AST_StatementNode *thenBranch, AST_StatementNode *elseBranch) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->condition != NULL  || node->nullCondition != NULL ||
       node->thenBranch != NULL || node->elseBranch != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->condition = condition;
    node->nullCondition = nullCondition;
    node->thenBranch = thenBranch;
    node->elseBranch = elseBranch;
} // AST_initNewIfNode()

/**
 * @brief Inicializuje uzel @c AST_WhileNode pro cyklus while.
 */
void AST_initNewWhileNode(AST_WhileNode *node, AST_ExprNode *condition, \
                          AST_VarNode *nullCondition, AST_StatementNode *body) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->condition != NULL || node->body != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->condition = condition;
    node->nullCondition = nullCondition;
    node->body = body;
} // AST_initNewWhileNode()

/**
 * @brief Inicializuje uzel @c AST_ExprNode pro výraz.
 */
void AST_initNewExprNode(AST_ExprNode *node, AST_ExprType exprType, void *expression) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->expression != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->exprType = exprType;
    node->expression = expression;
} // AST_initNewExprNode

/**
 * @brief Inicializuje uzel @c AST_BinOpNode pro binární operaci.
 */
void AST_initNewBinOpNode(AST_BinOpNode *node, AST_BinOpType op, \
                          AST_ExprNode *left, AST_ExprNode *right) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->left != NULL || node->right != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->op = op;
    node->left = left;
    node->right = right;
} // AST_initNewBinOpNode()

/**
 * @brief Inicializuje uzel @c AST_VarNode pro proměnnou nebo literál.
 */
void AST_initNewVarNode(AST_VarNode *node, AST_NodeType type, DString *identifier, \
                        size_t frameID, AST_LiteralType literalType, DString *value) {
    // Ověření platnosti předaného uzlu
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Pokud se nejedná o nový uzel beze zdrojů, dojde k interní chybě
    if(node->identifier != NULL || node->value != NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return;
    }

    // Přiřadíme uzlu předané zdroje
    node->type = type;
    node->identifier = identifier;
    node->frameID = frameID;
    node->literalType = literalType;

    // Pomocná proměnná pro funkce "strtol" a "strtod"
    char *endptr;       // Ukazatel na první neplatný znak po čísle

    // Převod hodnoty na příslušný datový typ na základě "literalType"
    switch(literalType) {
        // Obsah DString bude převeden na integer "i32"
        case AST_LITERAL_INT: {
            // Provádíme bezpečný převod řetězce na integer
            long intValue = strtol(value->str, &endptr, INT_CONVERTION_BASE);

            // Kontrola přetečení nebo podtečení - ostatní sémantická chyba (10)
            if(intValue < INT_MIN || intValue > INT_MAX) {
                parser_errorWatcher(SET_ERROR_SEM_OTHER);
                break;
            }

            // Kontrola neplatného vstupu - způsobeno špatnou tvorbou tokenu
            if(*endptr != '\0') {
                parser_errorWatcher(SET_ERROR_INTERNAL);
                break;
            }

            // Alokujeme paměť pro hodnotu
            int *intPtr = malloc(sizeof(int));
            if(intPtr == NULL) {
                parser_errorWatcher(SET_ERROR_INTERNAL);
                break;
            }

            // Převádíme řetězec na integer
            *intPtr = (int)intValue;
            node->value = intPtr;

            // Uvolníme úvodním obsah tokenu
            DString_free(value);
            value = NULL;

            break;
        } // case AST_LITERAL_INT

        // Obsah DString bude převeden na double "f64"
        case AST_LITERAL_FLOAT: {
            // Provádíme bezpečný převod řetězce na double
            double floatValue = strtod(value->str, &endptr);

            // Kontrola přetečení nebo podtečení - ostatní sémantická chyba (10)
            if(floatValue == HUGE_VAL || floatValue == -HUGE_VAL) {
                parser_errorWatcher(SET_ERROR_SEM_OTHER);
                break;
            }

            // Kontrola neplatného vstupu - způsobeno špatnou tvorbou tokenu
            if(*endptr != '\0') {
                parser_errorWatcher(SET_ERROR_INTERNAL);
                break;
            }

            // Alokujeme paměť pro hodnotu
            double *floatPtr = malloc(sizeof(double));
            if(floatPtr == NULL) {
                parser_errorWatcher(SET_ERROR_INTERNAL);
                break;
            }

            // Převádíme řetězec na double
            *floatPtr = (double)floatValue;
            node->value = floatPtr;

            // Uvolníme úvodním obsah tokenu
            DString_free(value);
            value = NULL;

            break;
        } // case AST_LITERAL_FLOAT

        // Obsah DString bude použit k reprezentaci řetězce
        case AST_LITERAL_STRING: {
            node->value = value;
            break;
        } // case AST_LITERAL_STRING

        // Hodnotou je NULL nebo není typ literálu
        // Pozn. Nad pseudo literálem typu boolean by neměla být funkce volána
        case AST_LITERAL_NULL:
        case AST_LITERAL_BOOL:
        case AST_LITERAL_NOT_DEFINED: {
            node->value = NULL;
            break;
        } // case AST_LITERAL_NULL, AST_LITERAL_BOOL, AST_LITERAL_NOT_DEFINED

        // Pokud byl předán jiný typ uzlu, nastává interní chyba
        default: {
            parser_errorWatcher(SET_ERROR_INTERNAL);
            break;
        } // default
    } // switch()
} // AST_initNewVarNode()


/*******************************************************************************
 *                                                                             *
 *           IMPLEMENTACE INTERNÍCH FUNKCÍ NA TVORBU A DESTRUKCI UZLŮ          *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Vytvoří kořenový uzel programu @c AST_ProgramNode.
 */
AST_ProgramNode *AST_createProgramNode() {
    // Alokujeme paměť pro nový uzel typu "AST_ProgramNode"
    AST_ProgramNode *node = (AST_ProgramNode *)malloc(sizeof(AST_ProgramNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_PROGRAM_NODE;
    node->importedFile = NULL;
    node->functionList = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createProgramNode()

/**
 * @brief Uvolní paměť pro kořenový uzel programu @c AST_ProgramNode.
 */
void AST_destroyProgramNode(AST_ProgramNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme proměnnou s cestou k importovanému souboru
    AST_destroyVarNode(node->importedFile);

    // Uvolníme všechny s uzlem svázané definice funkcí
    AST_destroyFunDefList(node->functionList);

    // Uvolníme uzel
    free(node);
} // AST_destroyProgramNode()

/**
 * @brief Vytvoří uzel pro definici funkce @c AST_FunDefNode.
 */
AST_FunDefNode *AST_createFunDefNode() {
    // Alokujeme paměť pro nový uzel typu "AST_FunDefNode"
    AST_FunDefNode *node = (AST_FunDefNode *)malloc(sizeof(AST_FunDefNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_FUN_DEF_NODE;
    node->identifier = NULL;
    node->parameters = NULL;
    node->returnType = AST_DATA_TYPE_NOT_DEFINED;
    node->body = NULL;
    node->next = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createFunDefNode()

/**
 * @brief Uvolní paměť pro uzel definice funkce @c AST_FunDefNode.
 */
void AST_destroyFunDefNode(AST_FunDefNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme dynamický řetězec uvnitř uzlu
    if(node->identifier != NULL) {
        DString_free(node->identifier);
        node->identifier = NULL;
    }

    // Uvolníme všechny s uzlem svázané parametry
    AST_destroyArgOrParamList(node->parameters);

    // Uvolníme všechny s uzlem svázané příkazy
    AST_destroyStatementList(node->body);

    // Uvolníme uzel
    free(node);
} // AST_destroyFunDefNode()

/**
 * @brief Vytvoří uzel pro parametry nebo argumenty funkce @c AST_ArgOrParamNode.
 */
AST_ArgOrParamNode *AST_createArgOrParamNode() {
    // Alokujeme paměť pro nový uzel typu "AST_ArgOrParamNode"
    AST_ArgOrParamNode *node = (AST_ArgOrParamNode *)malloc(sizeof(AST_ArgOrParamNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_ARG_OR_PARAM_NODE;
    node->dataType =  AST_DATA_TYPE_NOT_DEFINED;
    node->expression = NULL;
    node->next = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createArgOrParamNode()

/**
 * @brief Uvolní paměť pro uzel parametrů nebo argumentů funkce @c AST_ArgOrParamNode.
 */
void AST_destroyArgOrParamNode(AST_ArgOrParamNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme s předaným uzlem svázaný uzel proměnné
    AST_destroyNode(AST_EXPR_NODE, node->expression);

    // Uvolníme uzel
    free(node);
} // AST_destroyArgOrParamNode()

/**
 * @brief Vytvoří uzel pro příkaz @c AST_StatementNode.
 */
AST_StatementNode *AST_createStatementNode() {
    // Alokujeme paměť pro nový uzel typu "AST_StatementNode"
    AST_StatementNode *node = (AST_StatementNode *)malloc(sizeof(AST_StatementNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_STATEMENT_NODE;
    node->frameID = AST_FRAME_ID_NOT_ASSIGNED;
    node->statementType = AST_STATEMENT_NOT_DEFINED;
    node->statement = NULL;
    node->next = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createStatementNode()

/**
 * @brief Uvolní paměť pro uzel příkazu @c AST_StatementNode.
 */
void AST_destroyStatementNode(AST_StatementNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Pokud je ukazatel na konkrétní příkaz platný, ...
    if(node->statement != NULL) {
        // ...na základě jeho typu voláme specializovaný destruktor
        switch(node->statementType) {
            // Definice proměnné
            case AST_STATEMENT_VAR_DEF:
                AST_destroyExprNode(node->statement);
                break;

            // Výraz
            case AST_STATEMENT_EXPR:
                AST_destroyExprNode(node->statement);
                break;

            // Volání funkce
            case AST_STATEMENT_FUN_CALL:
                AST_destroyFunCallNode(node->statement);
                break;

            // Podmíněný příkaz "if"
            case AST_STATEMENT_IF:
                AST_destroyIfNode(node->statement);
                break;

            // Cyklus "while"
            case AST_STATEMENT_WHILE:
                AST_destroyWhileNode(node->statement);
                break;

            // Příkaz "return"
            case AST_STATEMENT_RETURN:
                AST_destroyExprNode(node->statement);
                break;

            // Ve výchozím stavu nebo pokud typ příkazu není definován se nic nestane
            case AST_STATEMENT_NOT_DEFINED:
            default:
                return;
        } // switch()
    } // if()

    // Uvolníme uzel
    free(node);
} // AST_destroyStatementNode()

/**
 * @brief Vytvoří uzel pro volání funkce @c AST_FunCallNode.
 */
AST_FunCallNode *AST_createFunCallNode() {
    // Alokujeme paměť pro nový uzel typu "AST_FunCallNode"
    AST_FunCallNode *node = (AST_FunCallNode *)malloc(sizeof(AST_FunCallNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_FUN_CALL_NODE;
    node->identifier = NULL;
    node->isBuiltIn = false;
    node->arguments = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createFunCallNode()

/**
 * @brief Uvolní paměť pro uzel volání funkce @c AST_FunCallNode.
 */
void AST_destroyFunCallNode(AST_FunCallNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme dynamický řetězec uvnitř uzlu
    if(node->identifier != NULL) {
        DString_free(node->identifier);
        node->identifier = NULL;
    }

    // Uvolníme všechny s uzlem svázané argumenty
    AST_destroyArgOrParamList(node->arguments);

    // Uvolníme uzel
    free(node);
} // AST_destroyFunCallNode()

/**
 * @brief Vytvoří uzel pro podmíněný příkaz if @c AST_IfNode.
 */
AST_IfNode *AST_createIfNode() {
    // Alokujeme paměť pro nový uzel typu "AST_IfNode"
    AST_IfNode *node = (AST_IfNode *)malloc(sizeof(AST_IfNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_IF_NODE;
    node->condition = NULL;
    node->nullCondition = NULL;
    node->thenBranch = NULL;
    node->elseBranch = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createIfNode()

/**
 * @brief Uvolní paměť pro uzel podmíněného příkazu if @c AST_IfNode.
 */
void AST_destroyIfNode(AST_IfNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme všechny s uzlem příkazu "if" svázané podmínky
    AST_destroyNode(AST_EXPR_NODE, node->condition);
    AST_destroyNode(AST_VAR_NODE, node->nullCondition);

    // Uvolníme všechny s větví "then" svázané příkazy
    AST_destroyStatementList(node->thenBranch);

    // Uvolníme všechny s větví "else" svázané příkazy
    AST_destroyStatementList(node->elseBranch);

    // Uvolníme uzel
    free(node);
} // AST_destroyIfNode()

/**
 * @brief Vytvoří uzel pro cyklus while @c AST_WhileNode.
 */
AST_WhileNode *AST_createWhileNode() {
    // Alokujeme paměť pro nový uzel typu "AST_WhileNode"
    AST_WhileNode *node = (AST_WhileNode *)malloc(sizeof(AST_WhileNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_WHILE_NODE;
    node->condition = NULL;
    node->nullCondition = NULL;
    node->body = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createWhileNode()

/**
 * @brief Uvolní paměť pro uzel cyklu while @c AST_WhileNode.
 */
void AST_destroyWhileNode(AST_WhileNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme všechny s uzlem cyklu "while" svázané podmínky
    AST_destroyNode(AST_EXPR_NODE, node->condition);
    AST_destroyNode(AST_VAR_NODE, node->nullCondition);

    // Uvolníme všechny s cyklem svázané příkazy
    AST_destroyStatementList(node->body);

    // Uvolníme uzel
    free(node);
} // AST_destroyWhileNode()

/**
 * @brief Vytvoří uzel pro výraz @c AST_ExprNode.
 */
AST_ExprNode *AST_createExprNode() {
    // Alokujeme paměť pro nový uzel typu "AST_ExprNode"
    AST_ExprNode *node = (AST_ExprNode *)malloc(sizeof(AST_ExprNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_EXPR_NODE;
    node->exprType = AST_EXPR_NOT_DEFINED;
    node->expression = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createExprNode()

/**
 * @brief Uvolní paměť pro uzel výrazu @c AST_ExprNode.
 */
void AST_destroyExprNode(AST_ExprNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Pokud je ukazatel na konkrétní výraz platný, ...
    if(node->expression != NULL) {
        // ...na základě jeho typu voláme specializovaný destruktor
        switch(node->exprType) {
            // Typ výrazu nebyl definován
            case AST_EXPR_NOT_DEFINED:
                break;

            // Literál
            case AST_EXPR_LITERAL:
                AST_destroyNode(AST_LITERAL_NODE, node->expression);
                break;

            // Proměnná (identifikátor)
            case AST_EXPR_VARIABLE:
                AST_destroyNode(AST_VAR_NODE, node->expression);
                break;

            // Volání funkce
            case AST_EXPR_FUN_CALL:
                AST_destroyNode(AST_FUN_CALL_NODE, node->expression);
                break;

            // Binární operace
            case AST_EXPR_BINARY_OP:
                AST_destroyNode(AST_BIN_OP_NODE, node->expression);
                break;

            // Jinak se nic nestane
            default:
                return;
        } // switch
    } // if()

    // Uvolníme uzel
    free(node);
} // AST_destroyExprNode()

/**
 * @brief Vytvoří uzel pro binární operaci @c AST_BinOpNode.
 */
AST_BinOpNode *AST_createBinOpNode() {
    // Alokujeme paměť pro nový uzel typu "AST_BinOpNode"
    AST_BinOpNode *node = (AST_BinOpNode *)malloc(sizeof(AST_BinOpNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Počáteční inicializace členů uzlu
    node->type = AST_BIN_OP_NODE;
    node->op = AST_OP_NOT_DEFINED;
    node->left = NULL;
    node->right = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createBinOpNode()

/**
 * @brief Uvolní paměť pro uzel binární operace @c AST_BinOpNode.
 */
void AST_destroyBinOpNode(AST_BinOpNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme levý a pravý operand binární operace
    AST_destroyNode(AST_EXPR_NODE, node->left);
    AST_destroyNode(AST_EXPR_NODE, node->right);

    // Uvolníme uzel
    free(node);
} // AST_destroyBinOpNode()

/**
 * @brief Vytvoří uzel pro proměnnou nebo literál @c AST_VarNode*.
 */
AST_VarNode *AST_createVarNode(AST_NodeType type) {
    // Alokujeme paměť pro nový uzel typu "AST_VarNode"
    AST_VarNode *node = (AST_VarNode *)malloc(sizeof(AST_VarNode));

    // Pokud se alokace nezdařila, hlásíme interní chybu překladače
    if(node == NULL) {
        parser_errorWatcher(SET_ERROR_INTERNAL);
        return NULL;
    }

    // Zvolíme, zda uzel reprezentuje proměnnou nebo literál
    if(type == AST_VAR_NODE) {
        node->type = AST_VAR_NODE;
    }
    else {
        node->type = AST_LITERAL_NODE;
    }

    // Počáteční inicializace členů uzlu
    node->identifier = NULL;
    node->frameID = AST_FRAME_ID_NOT_ASSIGNED;
    node->literalType = AST_LITERAL_NOT_DEFINED;
    node->value = NULL;

    // Vracíme ukazatel na nový uzel
    return node;
} // AST_createVarNode()

/**
 * @brief Uvolní paměť pro uzel proměnné nebo literál @c AST_VarNode*.
 */
void AST_destroyVarNode(AST_VarNode *node) {
    // Pokud je ukazatel na uzel neplatný, nic se nestane
    if(node == NULL) {
        return;
    }

    // Uvolníme dynamický řetězec uvnitř uzlu
    if(node->identifier != NULL) {
        DString_free(node->identifier);
        node->identifier = NULL;
    }

    // Pokud je ukazatel na příslušnou hodnotu proměnné/literálu platný, ...
    if(node->value != NULL) {
        // ...tak na základě jeho typu voláme specializovaný destruktor
        switch(node->literalType) {
            // Literál typu i32 a f64 a pseudo literál typu boolean
            case AST_LITERAL_INT:
            case AST_LITERAL_FLOAT:
            case AST_LITERAL_BOOL:
                free(node->value);
                break;

            // Literál typu []u8
            case AST_LITERAL_STRING:
                DString_free((DString *)(node->value));
                node->value = NULL;
                break;

            // Výchozí stav nebo literál typu NULL (pro ten neprovádíme žádnou akci)
            case AST_LITERAL_NULL:
            case AST_LITERAL_NOT_DEFINED:
            default:
                return;
        } // switch()
    } // if()

    // Uvolníme uzel
    free(node);
} // AST_destroyVarNode()


/*******************************************************************************
 *                                                                             *
 *         IMPLEMENTACE INTERNÍCH FUNKCÍ NA DESTRUKCI SEZNAMŮ UZLŮ AST         *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Uvolní paměť pro všechny uzly v seznamu parametrů/argumentů v uzlu pro
 *        funkci.
 */
void AST_destroyArgOrParamList(AST_ArgOrParamNode *list) {
    // Pokud byl předán neplatný ukazatel, nic se nestane
    if(list == NULL) {
        return;
    }

    // Vytvoříme si pomocný ukazatel na aktuální argument/parametr
    AST_ArgOrParamNode *currArgOrParamNode = list;

    // Cyklicky mažeme vždy první uzel pro argument/parametr v seznamu
    while(currArgOrParamNode != NULL) {
        AST_ArgOrParamNode *nextArgOrParamNode = currArgOrParamNode->next;
        AST_destroyArgOrParamNode(currArgOrParamNode);
        currArgOrParamNode = nextArgOrParamNode;
    }
} // AST_destroyArgOrParamList()

/**
 * @brief Uvolní paměť pro všechny uzly v seznamu pro příkazy.
 */
void AST_destroyStatementList(AST_StatementNode *list) {
    // Pokud byl předán neplatný ukazatel, nic se nestane
    if(list == NULL) {
        return;
    }

    // Vytvoříme si pomocný ukazatel na aktuální příkaz
    AST_StatementNode *currStatementNode = list;

    // Cyklicky mažeme vždy první uzel pro příkaz v seznamu
    while(currStatementNode != NULL) {
        AST_StatementNode *nextStatementNode = currStatementNode->next;
        AST_destroyStatementNode(currStatementNode);
        currStatementNode = nextStatementNode;
    }
} // AST_destroyStatementList()

/**
 * @brief Uvolní paměť pro všechny uzly v seznamu definovaných funkcí.
 */
void AST_destroyFunDefList(AST_FunDefNode *list) {
    // Pokud byl předán neplatný ukazatel, nic se nestane
    if(list == NULL) {
        return;
    }

    // Vytvoříme si pomocný ukazatel na aktuální definici funkce
    AST_FunDefNode *currFunDefNode = list;

    // Cyklicky mažeme vždy první uzel pro definici funkce v seznamu
    while(currFunDefNode != NULL) {
        AST_FunDefNode *nextFunDefNode = currFunDefNode->next;
        AST_destroyFunDefNode(currFunDefNode);
        currFunDefNode = nextFunDefNode;
    }
} // AST_destroyFunDefList()

/*** Konec souboru ast_interface.c ***/
