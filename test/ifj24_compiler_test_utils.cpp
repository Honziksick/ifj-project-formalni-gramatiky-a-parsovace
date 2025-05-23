/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           ast_test_utils.cpp                                        *
 * Autor:            Jan Kalina   <xkalinj00>                                  *
 *                                                                             *
 * Datum:            08.11.2024                                                *
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
 * @file ast_test_utils.cpp
 * @author Jan Kalina
 *
 * @brief Implementace utility funkcí pro práci s AST v testovacím prostředí.
 * @details Tento soubor obsahuje implementace funkcí pro tisk, porovnání a
 *          formátování různých typů uzlů AST. Implementace využívají escape
 *          sekvence pro zobrazení barevného výstupu, což usnadňuje vizualizaci
 *          struktury AST během testování.
 */

#include "ifj24_compiler_test_utils.h"

using namespace std;
using namespace testing;
using namespace internal;

string lexPath = "../test/test_examples/lexical_examples/";
string examPath = "../test/test_examples/ifj24code_examples/in/";
string syntPath = "../test/test_examples/syntactic_examples/";
string syntErrorPath = syntPath + "syntax_error/";
string semPath = "../test/test_examples/semantic_examples/";
string semErrorPath = semPath + "semantic_error/";

vector<bool> levels;

/*******************************************************************************
 *                                                                             *
 *                 IMPLEMENTACE VEŘEJNÝCH FUNKCÍ PRO VÝPIS AST                 *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Zachytí výstup tisku uzlu AST daného typu.
 */
string ASTutils_printCapturedOutput(AST_NodeType type, void *node, bool useColors) {
    stringstream buffer;

    if (node == nullptr) {
        buffer << "Null node passed" << endl;
        return buffer.str();
    }

    switch (type) {
        case AST_PROGRAM_NODE:
            ASTutils_printProgramNode((AST_ProgramNode *)node, buffer, 0, useColors, true);
            break;
        case AST_FUN_DEF_NODE:
            ASTutils_printFunDefNode((AST_FunDefNode *)node, buffer, 0, useColors, true);
            break;
        case AST_ARG_OR_PARAM_NODE:
            ASTutils_printArgOrParamNode((AST_ArgOrParamNode *)node, buffer, 0, useColors);
            break;
        case AST_STATEMENT_NODE:
            ASTutils_printStatementNode((AST_StatementNode *)node, buffer, 0, useColors);
            break;
        case AST_FUN_CALL_NODE:
            ASTutils_printFunCallNode((AST_FunCallNode *)node, buffer, 0, useColors, true);
            break;
        case AST_IF_NODE:
            ASTutils_printIfNode((AST_IfNode *)node, buffer, 0, useColors, true);
            break;
        case AST_WHILE_NODE:
            ASTutils_printWhileNode((AST_WhileNode *)node, buffer, 0, useColors, true);
            break;
        case AST_EXPR_NODE:
            ASTutils_printExprNode((AST_ExprNode *)node, buffer, 0, useColors, true);
            break;
        case AST_BIN_OP_NODE:
            ASTutils_printBinOpNode((AST_BinOpNode *)node, buffer, 0, useColors, true);
            break;
        case AST_VAR_NODE:
            ASTutils_printVarNode((AST_VarNode *)node, buffer, 0, useColors, true);
            break;
        case AST_LITERAL_NODE:
            ASTutils_printLiteralNode((AST_VarNode *)node, buffer, 0, useColors, true);
            break;
        default:
            buffer << "Unknown node type" << endl;
            break;
    }

    return buffer.str();
}  // ASTutils_printCapturedOutput()

/*******************************************************************************
 *                                                                             *
 *                 IMPLEMENTACE INTERNÍCH FUNKCÍ PRO VÝPIS AST                 *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Vytiskne uzel AST_NODE_PROGRAM.
 */
void ASTutils_printProgramNode(AST_ProgramNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if (node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Programový uzel je hlavní, tedy poslední uzel této úrovně
    ASTutils_printIndent(indent, out, isLastChild);
    if (useColors)
        out << COLOR_GOLD << "Program Node" << COLOR_RESET << endl;
    else
        out << "Program Node" << endl;

    // Aktualizace vektoru úrovní
    levels.push_back(true);

    // Ověření existence importovaného souboru
    if (node->importedFile != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Imported File: " << COLOR_RESET << endl;
        else
            out << "Imported File: " << endl;
        ASTutils_printVarNode(node->importedFile, out, indent + 2, useColors, true);
    } else {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Imported File: " << COLOR_RESET << "(null)" << endl;
        else
            out << "Imported File: " << "(null)" << endl;
    }

    // Aktualizace vektoru úrovní
    levels.pop_back();

    // Iterace přes seznam funkcí a tisk každé funkce
    AST_FunDefNode *func = node->functionList;
    if (func == nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Function List: " << COLOR_RESET << "(null)" << endl;
    } else {
        while (func != nullptr) {
            ASTutils_printFunDefNode(func, out, indent + 1, useColors, func->next == nullptr);
            func = func->next;
        }
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
} // ASTutils_printProgramNode()

/**
 * @brief Vytiskne uzel AST_NODE_FUN_DEF.
 */
void ASTutils_printFunDefNode(AST_FunDefNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení
    ASTutils_printIndent(indent, out, isLastChild);

    // Ověření existence identifikátoru
    if (node->identifier != nullptr && node->identifier->str != nullptr) {
        char *identifier = DString_DStringtoConstChar(node->identifier);
        if (useColors)
            out << COLOR_BLUE << "Function Definition: " << COLOR_RESET << identifier << endl;
        else
            out << "Function Definition: " << identifier << endl;
        free(identifier);
    } else {
        // Pokud `identifier` nebo `identifier->str` není platný, ošetříme výstup
        out << COLOR_BLUE << "Function Definition: " << COLOR_RESET << "(null)" << endl;
    }

    levels.push_back(!isLastChild);
    levels.push_back(!isLastChild);
    
    // Výpis parametrů funkce, pokud existují
    if(node->parameters != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Parameters:" << COLOR_RESET << endl;
        else
            out << "Parameters:" << endl;
        ASTutils_printArgOrParamNode(node->parameters, out, indent + 2, useColors);
    } else {
        // Pokud parametry neexistují
        ASTutils_printIndent(indent + 1, out, false);
        out << COLOR_BLUE << "Parameters: " << COLOR_RESET << "(null)" << endl;
    }
    // Aktualizace vektoru úrovní
    levels.pop_back();

    // Výpis návratového typu funkce
    ASTutils_printIndent(indent + 1, out, false);
    if (useColors)
        out << COLOR_BLUE << "Return Type: " << COLOR_RESET << node->returnType << endl;
    else
        out << "Return Type: " << node->returnType << endl;

    // Výpis těla funkce, pokud existuje
    if(node->body != nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Function Body:" << COLOR_RESET << endl;
        else
            out << "Function Body:" << endl;
        
        levels.push_back(false);
        ASTutils_printStatementNode(node->body, out, indent + 2, useColors);
        levels.pop_back();
    } else {
        // Pokud tělo funkce neexistuje
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Function Body: " << COLOR_RESET << "(null)" << endl;
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
}  // ASTutils_printFunDefNode()

/**
 * @brief Vytiskne uzel AST_NODE_ARG_OR_PARAM.
 */
void ASTutils_printArgOrParamNode(AST_ArgOrParamNode *node, ostream &out, int indent, bool useColors) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, true);
        out << "(null)" << endl;
        return;
    }

    // Iterace přes všechny parametry nebo argumenty
    while(node != nullptr) {
        // Příznak posledního výrazu v seznamu
        bool isLast = (node->next == nullptr);

        // Tisknutí odsazení
        ASTutils_printIndent(indent, out, isLast);

        // Aktualizace vektoru úrovní
        levels.push_back(!isLast);

        // Výpis typu parametru
        if (useColors)
            out << COLOR_BLUE << "Parameter Type: " << COLOR_RESET << node->dataType << endl;
        else
            out << "Parameter Type: " << node->dataType << endl;

        // Výpis výrazu parametru, pokud existuje
        if(node->expression != nullptr) {
            ASTutils_printExprNode(node->expression, out, indent + 1, useColors, true);
        } else {
            // Pokud výraz neexistuje
            ASTutils_printIndent(indent + 1, out, true);
            out << COLOR_BLUE << "Expression: " << COLOR_RESET << "(null)" << endl;
        }

        // Odstranit současnou úroveň z vektoru pro tisk svislice
        levels.pop_back();

        node = node->next;
    }
}   // ASTutils_printArgOrParamNode()

/**
 * @brief Vytiskne uzel AST_NODE_STATEMENT.
 */
void ASTutils_printStatementNode(AST_StatementNode *node, ostream &out, int indent, bool useColors) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, true);
        out << "(null)" << endl;
        return;
    }

    // Iterace přes všechny příkazy
    while(node != nullptr) {
        // Příznak posledního výrazu v seznamu
        bool isLast = (node->next == nullptr);

        // Tisknutí odsazení
        ASTutils_printIndent(indent, out, isLast);

        // Aktualizace vektoru úrovní
        levels.push_back(!isLast);

        if (useColors)
            out << COLOR_BLUE << "Statement: " << COLOR_RESET;
        else
            out << "Statement: ";
        
        // Rozlišení typu uzlu pomocí switch
        switch(node->statementType) {
            case AST_STATEMENT_VAR_DEF:
                if (useColors)
                    out << COLOR_GOLD << "Variable Definition" << COLOR_RESET << endl;
                else
                    out << "Variable Definition" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printExprNode((AST_ExprNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "Variable Definition: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            case AST_STATEMENT_EXPR:
                if (useColors)
                    out << COLOR_GOLD << "Expression" << COLOR_RESET << endl;
                else
                    out << "Expression" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printExprNode((AST_ExprNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "Expression: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            case AST_STATEMENT_FUN_CALL:
                if (useColors)
                    out << COLOR_GOLD << "Function Call" << COLOR_RESET << endl;
                else
                    out << "Function Call" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printFunCallNode((AST_FunCallNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "Function Call: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            case AST_STATEMENT_IF:
                if (useColors)
                    out << COLOR_GOLD << "If Statement" << COLOR_RESET << endl;
                else
                    out << "If Statement" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printIfNode((AST_IfNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "If Statement: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            case AST_STATEMENT_WHILE:
                if (useColors)
                    out << COLOR_GOLD << "While Loop" << COLOR_RESET << endl;
                else
                    out << "While Loop" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printWhileNode((AST_WhileNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "While Loop: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            case AST_STATEMENT_RETURN:
                if (useColors)
                    out << COLOR_GOLD << "Return Statement" << COLOR_RESET << endl;
                else
                    out << "Return Statement" << endl;
                if(node->statement != nullptr) {
                    ASTutils_printExprNode((AST_ExprNode*)node->statement, out, indent + 1, useColors, true);
                } else {
                    ASTutils_printIndent(indent + 1, out, true);
                    out << COLOR_BLUE << "Return Statement: " << COLOR_RESET << "(null)" << endl;
                }
                break;
            default:
                out << "(null)" << endl;
                break;
        }
        levels.pop_back();
        node = node->next;
    }
}  // ASTutils_printStatementNode()


/**
 * @brief Vytiskne uzel AST_NODE_FUN_CALL.
 */
void ASTutils_printFunCallNode(AST_FunCallNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení
    ASTutils_printIndent(indent, out, isLastChild);

    // Ověření existence identifikátoru a tisknutí identifikátoru
    if (node->identifier != nullptr && node->identifier->str != nullptr) { 
        char *identifier = DString_DStringtoConstChar(node->identifier);
        if (useColors)
            out << COLOR_GOLD << "Function Call: " << COLOR_RESET << identifier << endl;
        else
            out << "Function Call: " << identifier << endl;
        free(identifier);
    } else {
        // Pokud `identifier` nebo `identifier->str` není platný, ošetříme výstup
        out << COLOR_BLUE << "Function Call: " << COLOR_RESET << "(null)" << endl;
    }

    levels.push_back(!isLastChild);

    // Výpis argumentů funkce, pokud existují
    if(node->arguments != nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Arguments:" << COLOR_RESET << endl;
        else
            out << "Arguments:" << endl;
        ASTutils_printArgOrParamNode(node->arguments, out, indent + 2, useColors);
    } else {
        // Pokud argumenty neexistují
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Arguments: " << COLOR_RESET << "(null)" << endl;
    }


    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
}  // ASTutils_printFunCallNode()

/**
 * @brief Vytiskne uzel AST_NODE_IF.
 */
void ASTutils_printIfNode(AST_IfNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení a typu uzlu
    ASTutils_printIndent(indent, out, isLastChild);
    if (useColors)
        out << COLOR_GOLD << "If Statement" << COLOR_RESET << endl;
    else
        out << "If Statement" << endl;

    // Aktualizace vektoru úrovní
    levels.push_back(!isLastChild);
    levels.push_back(true);

    // Výpis podmínky if
    if(node->condition != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Condition:" << COLOR_RESET << endl;
        else
            out << "Condition:" << endl;
        ASTutils_printExprNode(node->condition, out, indent + 2, useColors, true);
    } else {
        // Pokud podmínka neexistuje
        ASTutils_printIndent(indent + 1, out, false);
        out << COLOR_BLUE << "Condition: " << COLOR_RESET << "(null)" << endl;
    }

    // Výpis nullCondition, pokud existuje
    if(node->nullCondition != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Null Condition:" << COLOR_RESET << endl;
        else
            out << "Null Condition:" << endl;
        ASTutils_printVarNode(node->nullCondition, out, indent + 2, useColors, true);
    }

    // Výpis větve 'then'
    if(node->thenBranch != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Then Branch:" << COLOR_RESET << endl;
        else
            out << "Then Branch:" << endl;
        ASTutils_printStatementNode(node->thenBranch, out, indent + 2, useColors);
    } else {
        // Pokud 'then' větev neexistuje
        ASTutils_printIndent(indent + 1, out, false);
        out << COLOR_BLUE << "Then Branch: " << COLOR_RESET << "(null)" << endl;
    }

    levels.pop_back();
    levels.push_back(false);

    // Výpis větve 'else', pokud existuje
    if(node->elseBranch != nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Else Branch:" << COLOR_RESET << endl;
        else
            out << "Else Branch:" << endl;
        ASTutils_printStatementNode(node->elseBranch, out, indent + 2, useColors);
    } else {
        // Pokud 'else' větev neexistuje
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Else Branch: " << COLOR_RESET << "(null)" << endl;
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
    levels.pop_back();
}  // ASTutils_printIfNode()

/**
 * @brief Vytiskne uzel AST_NODE_WHILE.
 */
void ASTutils_printWhileNode(AST_WhileNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení a typu uzlu
    ASTutils_printIndent(indent, out, isLastChild);
    if (useColors)
        out << COLOR_GOLD << "While Loop" << COLOR_RESET << endl;
    else
        out << "While Loop" << endl;

    // Aktualizace vektoru úrovní
    levels.push_back(!isLastChild);
    levels.push_back(true);

    // Výpis podmínky while
    if(node->condition != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Condition:" << COLOR_RESET << endl;
        else
            out << "Condition:" << endl;
        ASTutils_printExprNode(node->condition, out, indent + 2, useColors, true);
    } else {
        // Pokud podmínka neexistuje
        ASTutils_printIndent(indent + 1, out, false);
        out << COLOR_BLUE << "Condition: " << COLOR_RESET << "(null)" << endl;
    }

    // Výpis nullCondition, pokud existuje
    if(node->nullCondition != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);
        if (useColors)
            out << COLOR_BLUE << "Null Condition:" << COLOR_RESET << endl;
        else
            out << "Null Condition:" << endl;
        ASTutils_printVarNode(node->nullCondition, out, indent + 2, useColors, true);
    }

    levels.pop_back();
    levels.push_back(false);

    // Výpis těla cyklu
    if(node->body != nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Body:" << COLOR_RESET << endl;
        else
            out << "Body:" << endl;
        ASTutils_printStatementNode(node->body, out, indent + 2, useColors);
    } else {
        // Pokud tělo cyklu neexistuje
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Body: " << COLOR_RESET << "(null)" << endl;
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
    levels.pop_back();
}  // ASTutils_printWhileNode()

/**
 * @brief Vytiskne uzel AST_NODE_EXPR.
 */
void ASTutils_printExprNode(AST_ExprNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení a typu výrazu
    ASTutils_printIndent(indent, out, isLastChild);

    if (useColors)
        out << COLOR_BLUE << "Expression: " << COLOR_RESET;
    else
        out << "Expression: ";

    levels.push_back(!isLastChild);
    levels.push_back(!isLastChild);


    // Rozlišení typu výrazu pomocí switch
    switch(node->exprType) {
        case AST_EXPR_LITERAL:
            // Výraz je literál
            if (useColors)
                out << COLOR_GOLD << "Literal" << COLOR_RESET << endl;
            else
                out << "Literal" << endl;
            if(node->expression != nullptr) {
                // Aktualizace vektoru úrovní
                ASTutils_printVarNode((AST_VarNode*)node->expression, out, indent + 1, useColors, true);
            } else {
                ASTutils_printIndent(indent + 1, out, true);
                out << COLOR_BLUE << "Literal: " << COLOR_RESET << "(null)" << endl;
            }
            break;
        case AST_EXPR_VARIABLE:
            // Výraz je proměnná
            if (useColors)
                out << COLOR_GOLD << "Variable" << COLOR_RESET << endl;
            else
                out << "Variable" << endl;
            if(node->expression != nullptr) {
                // Aktualizace vektoru úrovní
                ASTutils_printVarNode((AST_VarNode*)node->expression, out, indent + 1, useColors, true);
            } else {
                ASTutils_printIndent(indent + 1, out, true);
                out << COLOR_BLUE << "Variable: " << COLOR_RESET << "(null)" << endl;
            }
            break;
        case AST_EXPR_FUN_CALL:
            // Výraz je volání funkce
            if (useColors)
                out << COLOR_GOLD << "Function Call" << COLOR_RESET << endl;
            else
                out << "Function Call" << endl;
            if(node->expression != nullptr) {
                // Aktualizace vektoru úrovní
                ASTutils_printFunCallNode((AST_FunCallNode*)node->expression, out, indent + 1, useColors, true);
            } else {
                ASTutils_printIndent(indent + 1, out, true);
                out << COLOR_BLUE << "Function Call: " << COLOR_RESET << "(null)" << endl;
            }
            break;
        case AST_EXPR_BINARY_OP:
            // Výraz je binární operace
            if (useColors)
                out << COLOR_GOLD << "Binary Operation" << COLOR_RESET << endl;
            else
                out << "Binary Operation" << endl;
            if(node->expression != nullptr) {
                // Aktualizace vektoru úrovní
                ASTutils_printBinOpNode((AST_BinOpNode*)node->expression, out, indent + 1, useColors, true);
            } else {
                ASTutils_printIndent(indent + 1, out, true);
                out << COLOR_BLUE << "Binary Operation: " << COLOR_RESET << "(null)" << endl;
            }
            break;
        default:
            // Neznámý typ výrazu
            out << "(null)" << endl;
            break;
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();
    levels.pop_back();
}  // ASTutils_printExprNode()

/**
 * @brief Vytiskne uzel AST_NODE_BIN_OP.
 */
void ASTutils_printBinOpNode(AST_BinOpNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení a operátoru
    ASTutils_printIndent(indent, out, isLastChild);
    if (useColors)
        out << COLOR_BLUE << "Operator: " << COLOR_RESET;
    else
        out << "Operator: ";
    
    // Aktualizace vektoru úrovní
    levels.push_back(!isLastChild);

    // Rozlišení typu operátoru pomocí switch
    switch(node->op) {
        case AST_OP_EQUAL:
            // Operátor rovnosti ==
            if (useColors)
                out << COLOR_GOLD << "==" << COLOR_RESET << endl;
            else
                out << "==" << endl;
            break;
        case AST_OP_NOT_EQUAL:
            // Operátor nerovnosti !=
            if (useColors)
                out << COLOR_GOLD << "!=" << COLOR_RESET << endl;
            else
                out << "!=" << endl;
            break;
        case AST_OP_LESS_THAN:
            // Operátor menší než <
            if (useColors)
                out << COLOR_GOLD << "<" << COLOR_RESET << endl;
            else
                out << "<" << endl;
            break;
        case AST_OP_GREATER_THAN:
            // Operátor větší než >
            if (useColors)
                out << COLOR_GOLD << ">" << COLOR_RESET << endl;
            else
                out << ">" << endl;
            break;
        case AST_OP_LESS_EQUAL:
            // Operátor menší nebo rovno <=
            if (useColors)
                out << COLOR_GOLD << "<=" << COLOR_RESET << endl;
            else
                out << "<=" << endl;
            break;
        case AST_OP_GREATER_EQUAL:
            // Operátor větší nebo rovno >=
            if (useColors)
                out << COLOR_GOLD << ">=" << COLOR_RESET << endl;
            else
                out << ">=" << endl;
            break;
        case AST_OP_ADD:
            // Operátor sčítání +
            if (useColors)
                out << COLOR_GOLD << "+" << COLOR_RESET << endl;
            else
                out << "+" << endl;
            break;
        case AST_OP_SUBTRACT:
            // Operátor odčítání -
            if (useColors)
                out << COLOR_GOLD << "-" << COLOR_RESET << endl;
            else
                out << "-" << endl;
            break;
        case AST_OP_MULTIPLY:
            // Operátor násobení *
            if (useColors)
                out << COLOR_GOLD << "*" << COLOR_RESET << endl;
            else
                out << "*" << endl;
            break;
        case AST_OP_DIVIDE:
            // Operátor dělení /
            if (useColors)
                out << COLOR_GOLD << "/" << COLOR_RESET << endl;
            else
                out << "/" << endl;
            break;
        case AST_OP_ASSIGNMENT:
            // Operátor přiřazení =
            if (useColors)
                out << COLOR_GOLD << "=" << COLOR_RESET << endl;
            else
                out << "=" << endl;
            break;
        default:
            // Neznámý operátor
            out << "(null)" << endl;
            break;
    }

    levels.pop_back();
    levels.push_back(isLastChild);
    // Výpis levého operandu, pokud existuje
    if(node->left != nullptr) {
        ASTutils_printIndent(indent + 1, out, false);

        if (useColors)
            out << COLOR_BLUE << "Left Operand:" << COLOR_RESET << endl;
        else
            out << "Left Operand:" << endl;
        // **Změna zde:** isLastChild = false pro levý operand
        ASTutils_printExprNode(node->left, out, indent + 2, useColors, true);
    } else {
        // Pokud levý operand neexistuje
        ASTutils_printIndent(indent + 1, out, false);
        out << COLOR_BLUE << "Left Operand: " << COLOR_RESET << "(null)" << endl;
    }


    levels.pop_back();

    // Výpis pravého operandu, pokud existuje
    if(node->right != nullptr) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Right Operand:" << COLOR_RESET << endl;
        else
            out << "Right Operand:" << endl;
        // **Změna zde:** isLastChild = true pro pravý operand
        ASTutils_printExprNode(node->right, out, indent + 2, useColors, true);
    } else {
        // Pokud pravý operand neexistuje
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Right Operand: " << COLOR_RESET << "(null)" << endl;
    }

}  // ASTutils_printBinOpNode()

/**
 * @brief Vytiskne uzel AST_NODE_LITERAL.
 */
void ASTutils_printLiteralNode(AST_VarNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení a typu literálu
    ASTutils_printIndent(indent, out, isLastChild);

    // Tisknutí typu literálu
    if (useColors)
        out << COLOR_BLUE << "Literal: " << COLOR_RESET;
    else
        out << "Literal: ";

    // Aktualizace vektoru úrovní
    levels.push_back(!isLastChild);

    char *str = nullptr;
    string escapedStr;
    // Rozlišení typu literálu pomocí switch
    switch(node->literalType) {
        case AST_LITERAL_INT:
            // Literál typu integer
            if(useColors) {
                if (node->value != nullptr)
                    out << COLOR_GOLD << "Integer: " << COLOR_RESET << *(int*)(node->value) << endl;
                else
                    out << COLOR_GOLD << "Integer: " << COLOR_RESET << "(null)" << endl;
            } else {
                if (node->value != nullptr)
                    out << "Integer:  " << *(int*)(node->value) << endl;
                else
                    out << "Integer: (null)" << endl;
            }
            break;
        case AST_LITERAL_FLOAT:
            // Literál typu float
            if(useColors) {
                double *value = (double*)(node->value);
                double print = *value;
                if (node->value != nullptr)
                    out << COLOR_GOLD << "Float: " << COLOR_RESET << print << endl;
                else
                    out << COLOR_GOLD << "Float: " << COLOR_RESET << "(null)" << endl;
            } else {
                if (node->value != nullptr)
                    out << "Float: " << *(double*)(node->value) << endl;
                else
                    out << "Float: (null)" << endl;
            }
            break;
        case AST_LITERAL_STRING:
            str = DString_DStringtoConstChar((DString*)node->value);
            // Literál typu string
            if(useColors) {
                if (node->value != nullptr) {
                    out << COLOR_GOLD << "String: " << COLOR_RESET << str << endl;
                }
                else
                    out << COLOR_GOLD << "String: " << COLOR_RESET << "(null)" << endl;
            } else {
                if (node->value != nullptr) {
                    out << "String: " << str << endl;
                }
                else
                    out << "String: (null)" << endl;
            }
            free(str);
            break;
        case AST_LITERAL_NULL:
            // Literál typu null
            if(useColors) {
                out << COLOR_GOLD << "Null" << COLOR_RESET << endl;
            } else {
                out << "Null" << endl;
            }
            break;
        default:
            // Neznámý typ literálu
            out << "Unknown Literal Type" << endl;
            break;
    }

    // Odstranit současnou úroveň z vektoru pro tisk svislice
    levels.pop_back();

}  // ASTutils_printLiteralNode()

/**
 * @brief Vytiskne uzel AST_NODE_VAR_DEF.
 */
void ASTutils_printVarNode(AST_VarNode *node, ostream &out, int indent, bool useColors, bool isLastChild) {
    if(node == nullptr) {
        ASTutils_printIndent(indent, out, isLastChild);
        out << "(null)" << endl;
        return;
    }

    // Tisknutí odsazení
    ASTutils_printIndent(indent, out, isLastChild);

    // Ověření existence identifikátoru a tisknutí identifikátoru
    if (node->identifier != nullptr && node->identifier->str != nullptr) { 
        char *identifier = DString_DStringtoConstChar(node->identifier);
        if (useColors)
            out << COLOR_BLUE << "Variable/Literal: " << COLOR_RESET << identifier << endl;
        else
            out << "Variable/Literal: " << identifier << endl;
        free(identifier);
    } else {
        // Pokud `identifier` nebo `identifier->str` není platný, ošetříme výstup
        out << COLOR_BLUE << "Variable/Literal: " << COLOR_RESET << "(null)" << endl;
    }

    ASTutils_printIndent(indent + 1, out, false);

    // Výpis FrameID
    if(useColors)
        out << COLOR_BLUE << "Frame ID: " << COLOR_RESET << node->frameID << endl;
    else
        out << "Frame ID: " << node->frameID << endl;

    // Výpis hodnoty proměnné, pokud existuje
    if(node->value != nullptr || node->literalType == AST_LITERAL_NULL) {
        ASTutils_printIndent(indent + 1, out, true);
        if (useColors)
            out << COLOR_BLUE << "Value:" << COLOR_RESET << endl;
        else
            out << "Value:" << endl;

        switch(node->literalType) {
            case AST_LITERAL_INT:
            case AST_LITERAL_FLOAT:
            case AST_LITERAL_STRING:
            case AST_LITERAL_NULL:
                ASTutils_printLiteralNode(node, out, indent + 2, useColors, true);
                break;
            default:
                ASTutils_printIndent(indent + 2, out, true);
                out << "Unknown Value Type" << endl;
                break;
        }
    } else {
        // Pokud `value` není platný, ošetříme výstup
        ASTutils_printIndent(indent + 1, out, true);
        out << COLOR_BLUE << "Value: " << COLOR_RESET << "(null)" << endl;
    }
} // ASTutils_printVarNode()

/**
 * @brief Pomocná funkce pro výpis odsazení
 */
void ASTutils_printIndent(int indent, ostream &out, bool isLastChild) {
    // Iterujte přes všechny úrovně kromě aktuální a předposlední
    for (int i = 0; i < indent-1; ++i) {
        if (i < (int)levels.size()) {
            if (levels[i])
                out << "│   ";
            else
                out << "    ";
        }
        else {
            out << "    ";
        }
    }

    if (indent > 0) {
        // Tiskněte aktuální uzel jako poslední nebo ne
        out << (isLastChild ? "└── " : "├── ");
    }
} // ASTutils_printIndent()

/*** Konec souboru ast_test_utils.cpp ***/
