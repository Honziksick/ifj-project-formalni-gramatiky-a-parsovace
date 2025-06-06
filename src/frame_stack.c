/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           frame_stack.c                                             *
 * Autor:            David Krejčí <xkrejcd00>                                  *
 *                                                                             *
 * Datum:            30.10.2024                                                *
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
 * @file frame_stack.c
 * @author David Krejčí \<xkrejcd00>
 *
 * @brief Implementační funkcí pro práci se zásobníkem rámců.
 * @details Tento implementační soubor obsahuje definice funkcí a datových struktur
 *          potřebných pro správu zásobníku rámců v překladači. Zásobník rámců
 *          je používán pro správu tabulek symbolů a jejich rámců během analýzy
 *          a generování kódu. Funkce v tomto souboru umožňují inicializaci,
 *          manipulaci a vyhledávání v zásobníku rámců.
 */

#include "frame_stack.h"

/*******************************************************************************
 *                                                                             *
 *                POČÁTEČNÍ INICIALIZACE GLOBÁLNÍCH PROMĚNNÝCH                 *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Globální zásobník rámců používaný v celém překladači.
 */
FrameStack frameStack = {NULL, NULL, 0};

/**
 * @brief Globální pole všech vytvořených rámců.
 */
FrameArray frameArray = {0, NULL};


/*******************************************************************************
 *                                                                             *
 *                        IMPLEMENTACE VEŘEJNÝCH FUNKCÍ                        *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Inicializuje zásobník rámců.
 */
void frameStack_init() {
    if(frameStack.top != NULL || frameStack.bottom != NULL) {
        return;
    }
    // Alokujeme místo pro globální rámec
    FramePtr frame = malloc(sizeof(Frame));
    if(frame == NULL) {
        error_handle(ERROR_INTERNAL);
    }

    // Vytvoříme tabulku symbolů pro globální rámec
    frame->frame = symtable_init();
    if(frame->frame == NULL) {
        free(frame);
        error_handle(ERROR_INTERNAL);
    }

    // Alokujeme pole rámců
    frameArray.array = malloc(FRAME_ARRAY_INIT_SIZE * sizeof(FramePtr));
    if(frameArray.array == NULL) {
        free(frame->frame);
        free(frame);
        error_handle(ERROR_INTERNAL);
    }

    // Nastavíme vrchol a spodek zásobníku na globální rámec
    frameStack.bottom = frame;
    frameStack.top = frame;

    // Inicializujeme globální rámec
    frame->frameID = 0;
    frame->next = NULL;
    frame->searchStop = true;

    // Inicializujeme pole rámců
    frameArray.allocated = FRAME_ARRAY_INIT_SIZE;
    frameArray.array[frame->frameID] = frame;

    // Do globálního rámce přidáme vestavěné funkce
    frameStack_addEmbeddedFunctions();
} // frameStack_init()

/**
 * @brief Vloží nový rámec na zásobník.
 */
void frameStack_push(bool isFunction) {
    // Pokud není zásobník inicializován, skončíme s chybou
    if(frameStack.top == NULL) {
        error_handle(ERROR_INTERNAL);
    }

    // Alokujeme místo pro rámec
    FramePtr frame = malloc(sizeof(Frame));
    if(frame == NULL) {
        error_handle(ERROR_INTERNAL);
    }

    // Vytvoříme tabulku symbolů rámce
    frame->frame = symtable_init();
    if(frame->frame == NULL) {
        free(frame);
        error_handle(ERROR_INTERNAL);
    }

    // Aktualizujeme nejvyšší ID
    frameStack.currentID++;

    // Nastavíme hodnoty rámce
    frame->frameID = frameStack.currentID;
    frame->next = frameStack.top;
    frame->searchStop = isFunction;

    // Nastavíme vrchol zásobníku
    frameStack.top = frame;

    // Pokud došla kapacita pole rámců, tak jej rozšíříme
    if(frameStack.currentID +1 > frameArray.allocated) {
        // Vytvoříme nové pole s dvojnásobnou kapacitou
        FramePtr *newArray = malloc(frameArray.allocated * FRAME_ARRAY_EXPAND_FACTOR * sizeof(FramePtr));
        if(newArray == NULL) {
            free(frame->frame);
            free(frame);
            error_handle(ERROR_INTERNAL);
        }

        // Překopírujeme položky
        memcpy(newArray, frameArray.array, frameArray.allocated * sizeof(FramePtr));

        // Uvolníme staré pole
        free(frameArray.array);

        // Nastavíme novou velikost a ukazatel na pole
        frameArray.allocated *= FRAME_ARRAY_EXPAND_FACTOR;
        frameArray.array = newArray;
    }

    // Přidáme rámec do globálního pole
    frameArray.array[frame->frameID] = frame;
} // frameStack_push()

/**
 * @brief Odstraní vrchní rámec ze zásobníku.
 */
FrameStack_result frameStack_pop() {
    if(frameStack.top == NULL) {
        return FRAME_STACK_NOT_INITIALIZED;
    }

    if(frameStack.top->next == NULL) {
        return FRAME_STACK_POP_GLOBAL;
    }

    frameStack.top = frameStack.top->next;

    return FRAME_STACK_SUCCESS;
} // frameStack_pop()

/**
 * @brief Vyhledá položku v zásobníku rámců podle klíče.
 */
FrameStack_result frameStack_findItem(DString *key, SymtableItem **outItem) {
    // Pokud je klíč NULL, vrátíme chybu
    if(key == NULL) {
        return FRAME_STACK_KEY_NULL;
    }

    // Pokud není zásobník inicializován, vrátíme chybu
    if(frameStack.top == NULL) {
        return FRAME_STACK_NOT_INITIALIZED;
    }

    // Nastavíme začátek prohledávání na vrchol zásobníku
    FramePtr frame = frameStack.top;

    // Cyklus prohledávání rámců
    while(true) {
        // Prohledáme rámec
        Symtable_result result = symtable_findItem(frame->frame, key, outItem);
        if(result == SYMTABLE_SUCCESS) {
            // Pokud byla položka nalezena, vrátíme úspěch
            return FRAME_STACK_SUCCESS;
        }
        else {
            // Pokud položka nebyla nalezena a vynořili bychom se z funkce,
            // položka neexistuje
            if(frame->searchStop) {
                // Pokud jsme na globálním rámci, položka neexistuje
                if(frame->frameID == 0) {
                    return FRAME_STACK_ITEM_DOESNT_EXIST;
                }
                break;
            }

            // Jinak pokračujeme v prohledávání dalšího rámce
            frame = frame->next;
        }
    }

    // Projdeme ještě globální rámec
    Symtable_result result = symtable_findItem(frameArray.array[0]->frame, key, outItem);
    if(result == SYMTABLE_SUCCESS) {
        return FRAME_STACK_SUCCESS;
    }
    else {
        return FRAME_STACK_ITEM_DOESNT_EXIST;
    }
} // frameStack_findItem()

/**
 * @brief Přidá novou položku do vrchního rámce zásobníku.
 */
FrameStack_result frameStack_addItem(DString *key, SymtableItem **outItem) {
    // Voláme findItem, abychom zjistili, zda položka již existuje
    // Zároveň získáme ukazatel na položku, pokud existuje
    // Funkce se zároveň postará o chybové stavy
    FrameStack_result result = frameStack_findItem(key, outItem);

    // Pokud položka již existuje, vracíme tuto informaci
    if(result == FRAME_STACK_SUCCESS) {
        return FRAME_STACK_ITEM_ALREADY_EXISTS;
    }

    // Pokud položka neexistuje, přidáme ji
    if(result == FRAME_STACK_ITEM_DOESNT_EXIST) {
        Symtable_result sym_result = symtable_addItem(frameStack.top->frame, key, outItem);

        // Pokud se položka podařilo přidat, vrátíme úspěch
        if(sym_result == SYMTABLE_SUCCESS) {
            return FRAME_STACK_SUCCESS;
        }
        else {
            // Pokud selhalo přidání položky, vrátíme chybu
            return FRAME_STACK_ALLOCATION_FAIL;
        }
    }

    // Pokud došlo k chybě při hledání, vrátíme chybu
    return result;
} // frameStack_addItem()

/**
 * @brief Přidá novou položku s daty do vrchního rámce zásobníku.
 */
FrameStack_result frameStack_addItemExpress(DString *key,
                    Symtable_symbolState state, bool constant, void* data, SymtableItem **outItem) {
    SymtableItemPtr item;
    FrameStack_result result = frameStack_addItem(key, &item);
    if(result != FRAME_STACK_SUCCESS) {
        return result;
    }

    item->symbolState = state;
    item->constant = constant;
    item->data = data;

    if(outItem != NULL) {
        *outItem = item;
    }

    return FRAME_STACK_SUCCESS;
} // frameStack_addItemExpress()

/**
 * @brief Vrátí ID rámce, ve kterém je položka s daným klíčem.
 */
size_t frameStack_getId(DString *key) {
    // Pokud je klíč NULL, vrátíme chybu
    if(key == NULL) {
        return FRAME_STACK_KEY_NULL;
    }

    // Pokud není zásobník inicializován, vrátíme chybu
    if(frameStack.top == NULL) {
        return FRAME_STACK_NOT_INITIALIZED;
    }

    // Nastavíme začátek prohledávání na vrchol zásobníku
    FramePtr frame = frameStack.top;

    // Cyklus prohledávání rámců
    while(true) {
        // Prohledáme rámec
        Symtable_result result = symtable_findItem(frame->frame, key, NULL);
        if(result == SYMTABLE_SUCCESS) {
            // Pokud byla položka nalezena, vrátíme úspěch
            return frame->frameID;
        }
        else {
            // Jinak pokračujeme v prohledávání dalšího rámce
            frame = frame->next;
        }
    }

    return 0;
} // frameStack_getId()

/**
 * @brief Uvolní všechny rámce v zásobníku a uvede zásobník do počátečního stavu.
 */
void frameStack_destroyAll() {
    // Pokud není zásobník inicializován, vrátíme se
    if(frameStack.top == NULL) {
        return;
    }

    // Uvolníme všechny rámce
    for(size_t i = 0; i <= frameStack.currentID; i++) {
        symtable_destroyTable(frameArray.array[i]->frame);
        free(frameArray.array[i]);
    }
    free(frameArray.array);

    // Nastavíme zásobník na původní stav
    frameStack.bottom = NULL;
    frameStack.top = NULL;
    frameStack.currentID = 0;

    // Nastavíme pole rámců na původní stav
    frameArray.allocated = 0;
    frameArray.array = NULL;
} // frameStack_destroyAll()

/**
 * @brief Vytiskne obsah pole rámců
 */
void frameStack_printArray(FILE *file, bool printData, bool cutData) {
    for(size_t i = 0; i <= frameStack.currentID; i++) {
        if(frameArray.array == NULL) {
            return;
        }

        fprintf(file, "Frame ID: %-10zu", (frameArray.array[i])->frameID);

        if((frameArray.array[i])->searchStop) {
            fprintf(file, "is searchStop frame");
        }

        fprintf(file, "\n");
        symtable_print((frameArray.array[i])->frame, file, printData, cutData);
        fprintf(file, "\n");

    }
} // frameStack_printArray()

/**
 * @brief Vytiskne obsah zásobníku rámců
 */
void frameStack_print(FILE *file, bool printData, bool cutData) {
    // Pokud není zásobník inicializován, vrátíme se
    if(frameStack.top == NULL) {
        return;
    }
    // Vytiskneme všechny rámce
    FramePtr frame = frameStack.top;
    while(frame != NULL) {
        fprintf(file, "Frame ID: %-10zu", frame->frameID);

        if(frame->searchStop) {
            fprintf(file, "is searchStop frame");
        }

        fprintf(file, "\n");
        symtable_print(frame->frame, file, printData, cutData);
        fprintf(file, "\n");

        frame = frame->next;
    }
} // frameStack_print()

/**
 * @brief Vytiskne obsah zásobníku rámců
 */
inline void frameStack_printSimple() {
    frameStack_print(stdout, false, true);
} // frameStack_printSimple()

void frameStack_addEmbeddedFunctions() {
    SymtableFunctionData *data;

    // Funkce pro načítání hodnot
    data = symtable_initFunctionData(0);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_STRING_OR_NULL;
    if(frameStack_addFunction("ifj.readstr", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(0);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_INT_OR_NULL;
    if(frameStack_addFunction("ifj.readi32", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(0);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_DOUBLE_OR_NULL;
    if(frameStack_addFunction("ifj.readf64", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    // Funkce pro výpis hodnoty
    // Neuložíme skutečná data, protože bere jakýkoliv typ = speciální případ
    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_VOID;
    data->params[0].id = DString_constCharToDString("term");
    if(frameStack_addFunction("ifj.write", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    // Funkce pro konverzi číselných typů
    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_DOUBLE;
    data->params[0].type = SYMTABLE_TYPE_INT;
    data->params[0].id = DString_constCharToDString("term");
    if(frameStack_addFunction("ifj.i2f", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_INT;
    data->params[0].type = SYMTABLE_TYPE_DOUBLE;
    data->params[0].id = DString_constCharToDString("term");
    if(frameStack_addFunction("ifj.f2i", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    // Funkce pro práci s řezy
    // Neuložíme skutečná data, protože bere 2 typy = speciální případ
    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("term");
    if(frameStack_addFunction("ifj.string", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_INT;
    data->params[0].type = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("s");
    if(frameStack_addFunction("ifj.length", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(2);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_STRING;
    data->params[0].type = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("s1");
    data->params[1].type = SYMTABLE_TYPE_STRING;
    data->params[1].id = DString_constCharToDString("s2");
    if(frameStack_addFunction("ifj.concat", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(3);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_STRING_OR_NULL;
    data->params[0].type = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("s");
    data->params[1].type = SYMTABLE_TYPE_INT;
    data->params[1].id = DString_constCharToDString("i");
    data->params[2].type = SYMTABLE_TYPE_INT;
    data->params[2].id = DString_constCharToDString("j");
    if(frameStack_addFunction("ifj.substring", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(2);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_INT;
    data->params[0].type = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("s1");
    data->params[1].type = SYMTABLE_TYPE_STRING;
    data->params[1].id = DString_constCharToDString("s2");
    if(frameStack_addFunction("ifj.strcmp", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(2);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_INT;
    data->params[0].type = SYMTABLE_TYPE_STRING;
    data->params[0].id = DString_constCharToDString("s");
    data->params[1].type = SYMTABLE_TYPE_INT;
    data->params[1].id = DString_constCharToDString("i");
    if(frameStack_addFunction("ifj.ord", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }

    data = symtable_initFunctionData(1);
    if(data == NULL) {
        return;
    }
    data->returnType = SYMTABLE_TYPE_STRING;
    data->params[0].type = SYMTABLE_TYPE_INT;
    data->params[0].id = DString_constCharToDString("i");
    if(frameStack_addFunction("ifj.chr", data) != FRAME_STACK_SUCCESS) {
        error_handle(ERROR_INTERNAL);
    }
} // frameStack_addEmbeddedFunctions()

FrameStack_result frameStack_addFunction(const char* key, void* data) {
    // Funkce pro načítání hodnot
    DString *sKey = DString_constCharToDString(key);
    if(key == NULL) {
        return FRAME_STACK_ALLOCATION_FAIL;
    }
    SymtableItemPtr item;
    FrameStack_result result = frameStack_addItem(sKey, &item);
    if(result != FRAME_STACK_SUCCESS) {
        DString_free(sKey);
        return result;
    }
    item->symbolState = SYMTABLE_SYMBOL_FUNCTION;
    item->data = data;
    DString_free(sKey);
    return FRAME_STACK_SUCCESS;
} // frameStack_addFunction()

/*** Konec souboru frame_stack.c ***/
