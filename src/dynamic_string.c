/*******************************************************************************
 *                                                                             *
 * Název projektu:   Implementace překladače imperativního jazyka IFJ24        *
 *                                                                             *
 * Soubor:           dynamic_string.c                                          *
 * Autor:            Farkašovský Lukáš  <xfarkal00> (hlavní)                   *
 *                   Kalina Jan         <xkalinj00> (edit)                     *
 *                                                                             *
 * Datum:            08.10.2024                                                *
 * Poslední změna:   27.11.2024                                                *
 *                                                                             *
 * Tým:      Tým xkalinj00                                                     *
 * Členové:  Farkašovský Lukáš    <xfarkal00>                                  *
 *           Hýža Pavel           <xhyzapa00>                                  *
 *           Kalina Jan           <xkalinj00>                                  *
 *           Krejčí David         <xkrejcd00>                                  *
 *                                                                             *
*******************************************************************************/
/**
 * @file dynamic_string.c
 * @author Farkašovský Lukáš \<xfarkal00> (hlavní)
 * @author Kalina Jan \<xkalinj00> (edit)
 *
 * @brief Implementace funkcí pro práci s dynamickým řetězcem DString.
 * @details Tento soubor obsahuje deklaraci funkcí a datových typů knihovny
 *          pro dynamický řetězec.
*/

#include "dynamic_string.h"


/*******************************************************************************
 *                                                                             *
 *                        IMPLEMENTACE VEŘEJNÝCH FUNKCÍ                        *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Inicializace dynamického řetězce s počáteční kapacitou.
*/
DString *DString_init() {
    // Vytvoříme nový string
    DString *stringCreated = (DString *)malloc(sizeof(DString));

    // Pokud se špatně alokovala paměť, vrátíme NULL
    if(stringCreated == NULL) {
        return NULL;
    }

    // Vytvoříme buňky pro znaky
    stringCreated->str = (char *)malloc(STRING_INIT_SIZE * sizeof(char));

    // Pokud se špatně alokovala paměť, vrátíme NULL
    if(stringCreated->str == NULL) {
        free(stringCreated);
        return NULL;
    }

    // Na začátku není v řetězci žádný znak a alokováno máme STRING_INIT_SIZE znaků
    for(int i = 0; i < STRING_INIT_SIZE; i++){
        stringCreated->str[i] = '\0';
    }
    stringCreated->allocatedSize = STRING_INIT_SIZE;
    stringCreated->length = 0;

    return stringCreated;
} // konec DString_init()

/**
 * @brief Uvolnění paměti dynamického řetězce.
*/
void DString_free(DString *str) {
    // Pokud řetězec existuje, smažeme ho
    if(str != NULL) {
        // Nejdříve ukazatel na pole a až pak strukturu
        if(str->str != NULL) {
            free(str->str);
            str->allocatedSize = 0;
            str->length = 0;
            str->str = NULL;
        }

        // Nakonec uvolníme celý DString
        free(str);
        str = NULL;
    }
} // konec DString_free()

/**
 * @brief Přidání jednoho znaku na konec dynamického řetězce.
*/
int DString_appendChar(DString *str, char character) {
    // Kontrola, že přijatý ukazatel není NULL
    if(str == NULL) {
        return STRING_RESIZE_FAIL;
    }

    // Pokud je řetězec plný, musíme ho zvětšit o DEFAULT_RESIZE_SIZE
    if(str->length+1 >= str->allocatedSize) {
        // Kontrola, zda neselhalo zvětšení řetězce
        str = DString_resize(str, DEFAULT_RESIZE_SIZE);
        if(str == NULL){
            return STRING_RESIZE_FAIL;
        }
    }

    // Přidání znaku na konec
    str->str[str->length] = character;
    // Délka řetězce se zvětší o jeden
    str->length++;

    return STRING_SUCCESS;
} // konec DString_appendChar()

/**
 * @brief Zkopíruje obsah jednoho dynamického řetězce do druhého.
*/
int DString_copy(DString *strCopied, DString *strTo) {
    // Pokud jeden z řetězců neexistuje, vrátí STRING_COPY_FAIL
    if(strCopied == NULL || strTo == NULL) {
        return STRING_COPY_FAIL;
    }

    // Uvolníme buňky řetězce, do kterého chceme kopírovat (vyčistíme ho)
    free(strTo->str);

    // Alokujeme nový dynamický řetězec (+1 pro nulový znak)
    strTo->str = (char *)malloc((strCopied->length + 1) * sizeof(char));
    if(strTo->str == NULL) {
        return STRING_COPY_FAIL;
    }

    // Zkopírujeme obsah zdrojového řetězce do cílového
    memcpy(strTo->str, strCopied->str, strCopied->length);

    // Přidáme nulový znak na konec řetězce
    strTo->str[strCopied->length] = '\0';

    // Délka i alokovaná paměť zkopírovaného řetězce odpovídá délce původního
    strTo->length = strCopied->length;
    strTo->allocatedSize = strCopied->length + 1;

    return STRING_SUCCESS;
} // konec DString_copy()

/**
 * @brief Porovná dva dynamické řetězce.
*/
int DString_compare(DString *str1, DString *str2) {
    // Pokud je jeden ze řetězců prázdný, vrátíme STRING_NOT_EQUAL
    if(str1 == NULL || str2 == NULL) {
        return STRING_NOT_EQUAL;
    }

    /* Pokud od začátku víme, že jsou růžně dlouhé oba řetězce,
       potom nemohou být stejné.*/
    if(str1->length != str2->length) {
        return STRING_NOT_EQUAL;
    }

    // Projdeme oba řetězce
    for(size_t i = 0; i < str1->length; i++) {
        // Pokud si jsou odlišné, vrátíme STRING_NOT_EQUAL
        if(str1->str[i] != str2->str[i]){
            return STRING_NOT_EQUAL;
        }
    }
    return STRING_EQUAL;
} // konec DString_compare()

/**
 * @brief Porovná dynamický řetězec s konstantním řetězcem.
*/
int DString_compareWithConstChar(DString *str, const char *strConst) {
    // Pokud porovnávaný řetězec neexistuje
    if(str == NULL) {
        return STRING_NOT_EQUAL;
    }
    // Proměnná pro uložení délky strConst
    size_t length = strlen(strConst);

    // Pokud nejsou stejně dlouhé, tak nejsou ani stejné
    if(str->length != length) {
        return STRING_NOT_EQUAL;
    }

    // Projdeme oba řetězce a porovnáme je
    for(size_t i = 0; i < str->length; i++) {
        // Pokud nejsou stejné vracíme STRING_NOT_EQUAL
        if(str->str[i] != strConst[i]) {
            return STRING_NOT_EQUAL;
        }
    }

    return STRING_EQUAL;
} // konec DString_compareWithConstChar()

/**
 * @brief Převede řetězec obsažený v DString na konstantní řetězec typu char.
 */
char *DString_DStringtoConstChar(DString *string) {
    // Pokud nemáme řetězec, nebo nemá hodnotu, tak vracíme NULL
    if(string == NULL || string->str == NULL || string->length < 1) {
        return NULL;
    }

    // Alokace paměti pro nový konstantní řetězec (+1 pro nulový znak)
    char *constStr = (char *)malloc((string->length + 1) * sizeof(char));
    if(constStr == NULL) {
        return NULL;
    }

    // Kopírování hodnoty DString do nového řetězce
    for(size_t i = 0; i < string->length; i++) {
        constStr[i] = string->str[i];
    }
    // Nulový znak na konec řetězce
    constStr[string->length] = '\0';

    return constStr;
} // konec DString_DStringtoConstChar()


/**
 * @brief Vytvoří nový dynamický řetězec z konstantního řetězce.
 */
DString *DString_constCharToDString(const char *strConst){
    // Pokud nemáme řetězec, nebo nemá hodnotu, tak vracíme NULL
    if(strConst == NULL || strlen(strConst) < 1) {
        return NULL;
    }

    // Získáme délku konstantního řetězce
    size_t length = strlen(strConst);

    // Vytvoříme nový DString
    DString *stringCreated = (DString *)malloc(sizeof(DString));

    // Pokud se špatně alokovala paměť, vrátíme NULL
    if(stringCreated == NULL) {
        return NULL;
    }

    // Vytvoříme buňky pro znaky
    stringCreated->str = (char *)malloc( ((length+1)) * sizeof(char) );

    // Pokud se špatně alokovala paměť, vrátíme NULL
    if(stringCreated->str == NULL) {
        free(stringCreated);
        return NULL;
    }

    // Nastavíme délku a alokovanou paměť
    stringCreated->length = length;
    stringCreated->str[length] = '\0';
    stringCreated->allocatedSize = length+1;

    // Překopírujeme obsah konstantního řetězce do dynamického
    memcpy(stringCreated->str, strConst, length*sizeof(char));

    return stringCreated;
} // konec DString_constCharToDString()


/*******************************************************************************
 *                                                                             *
 *                        IMPLEMENTACE INTERNÍCH FUNKCÍ                        *
 *                                                                             *
 ******************************************************************************/

/**
 * @brief Zvětší dynamický řetězec na požadovanou délku.
 */
DString *DString_resize(DString *string, size_t size) {
    // Pokud nemáme řetězec, tak vracíme NULL
    if(string == NULL) {
        return NULL;
    }

    // Nový paměťový nárok pole znaků dynamického stringu
    size_t resizeSize = string->length + size;

    // Vytvoříme buňky pro znaky
    char *newStr = (char *)realloc(string->str, resizeSize * sizeof(char));

    // Pokud se špatně realokuje, vrací NULL
    if(string->str == NULL) {
        free(string);
        return NULL;
    }

    // Aktualizace ukazatele na pole znaků a velikost alokované paměti
    string->str = newStr;
    string->allocatedSize = resizeSize;
    for(size_t i = string->length; i < resizeSize; i++){
        string->str[i] = '\0';
    }

    return string;
} // konec DString_resize()

/*** Konec souboru dynamic_string.c ***/
