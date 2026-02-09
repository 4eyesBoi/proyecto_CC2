#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cwctype>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define IDC_TREEVIEW 1001
#define IDC_RICHTEXT 1002

HWND hMainWindow;
HWND hTreeView;
HWND hTextDisplay;

// Guardar strings para asegurar que los punteros a texto permanezcan válidos
static std::vector<std::wstring> g_treeStrings;

// Controles y estados para la zona de detalles
#define ID_BTN_CREATE 2001
#define ID_BTN_INSERT 2002
#define ID_BTN_SAVE   2003
#define ID_BTN_LOAD   2004
#define ID_BTN_PRINT  2005
#define ID_BTN_SEARCH 2006
#define ID_BTN_DELETE 2007

HWND hBtnCreate = NULL;
HWND hBtnInsert = NULL;
HWND hBtnSave = NULL;
HWND hBtnLoad = NULL;
HWND hBtnPrint = NULL;
HWND hBtnSearch = NULL;
HWND hBtnDelete = NULL;
HWND hListArray = NULL;

// (removed details brush — no colored background)

enum Mode { MODE_NONE=0, MODE_SEQUENTIAL=1, MODE_BINARY=2 };
Mode currentMode = MODE_NONE;

enum Action { ACTION_NONE=0, ACT_CREATE, ACT_INSERT, ACT_SEARCH, ACT_DELETE, ACT_SAVE, ACT_LOAD, ACT_PRINT };
Action currentAction = ACTION_NONE;

HWND hActionLabel1 = NULL;
HWND hActionEdit1 = NULL;
HWND hActionLabel2 = NULL;
HWND hActionEdit2 = NULL;
HWND hActionOk = NULL;
HWND hActionCancel = NULL;

struct DataArray {
    std::vector<std::wstring> keys;
    int keyDigits = 0;
    bool created = false;
};

static DataArray currentArray;

// Helpers
std::wstring PromptModal(HWND hParent, const std::wstring& title, const std::wstring& prompt);
void UpdateArrayListDisplay();
void ShowControlsForMode(Mode mode);
void HideDetailControls();
void DoCreateArray();
void DoInsert();
void DoSave();
void DoLoad();
void DoSearch();
void DoDelete();
// Estructura para los nodos del árbol
struct TreeNode {
    std::wstring text;
    std::wstring content;
    int imageIndex;
};

// Prototipos
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitializeTreeView(HWND hwnd);
void PopulateTreeView();
HTREEITEM AddTreeItem(HWND hwndTV, HTREEITEM hParent, const std::wstring& text);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Inicializar controles comunes
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TREEVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Registrar la clase de ventana
    const wchar_t CLASS_NAME[] = L"ALGORITMOS_APP";
    WNDCLASSW wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));

    RegisterClassW(&wc);

    // Crear ventana principal
    hMainWindow = CreateWindowW(
        CLASS_NAME,
        L"Algoritmos de Búsqueda - Navegador de Contenido",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100, 100,
        1200, 700,
        NULL, NULL,
        hInstance, NULL
    );

    if (hMainWindow == NULL) {
        return 0;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    // Bucle de mensajes
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            InitializeTreeView(hwnd);
            return 0;

        case WM_SIZE: {
                    int width = LOWORD(lParam);
                    int height = HIWORD(lParam);

                    // Usar ~40% del ancho para el TreeView para que los textos quepan
                    int treeWidth = (width * 40) / 100;
                    MoveWindow(hTreeView, 10, 10, treeWidth - 15, height - 20, TRUE);

                    // Ajustar tamaño del área de texto (solo los primeros ~100 píxeles para no ocluir botones)
                    MoveWindow(hTextDisplay, treeWidth + 5, 10, width - treeWidth - 25, 100, TRUE);
                    
                    // Enviar botones y controles al frente
                    SetWindowPos(hBtnCreate, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnInsert, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnSearch, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnDelete, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnSave, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnLoad, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    SetWindowPos(hBtnPrint, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                    
            return 0;
        }

        case WM_NOTIFY: {
            LPNMTREEVIEWW pnmtv = (LPNMTREEVIEWW)lParam;

            if (pnmtv->hdr.hwndFrom == hTreeView) {
                if (pnmtv->hdr.code == TVN_SELCHANGEDW) {
                    // Un nodo fue seleccionado
                    TVITEMW item;
                    wchar_t buffer[256];

                    item.hItem = pnmtv->itemNew.hItem;
                    item.mask = TVIF_TEXT;
                    item.pszText = buffer;
                    item.cchTextMax = sizeof(buffer) / sizeof(buffer[0]);

                    if (TreeView_GetItem(hTreeView, &item)) {
                        std::wstring sel(buffer);
                        // Trim whitespace for comparison
                        size_t start = sel.find_first_not_of(L" \t\r\n");
                        size_t end = sel.find_last_not_of(L" \t\r\n");
                        if (start != std::wstring::npos) {
                            sel = sel.substr(start, end - start + 1);
                        }
                        
                        // Obtener el nodo padre para saber si es Secuencial o Binaria
                        HTREEITEM hParent = TreeView_GetParent(hTreeView, pnmtv->itemNew.hItem);
                        std::wstring parentName = L"";
                        if (hParent) {
                            TVITEMW parentItem;
                            wchar_t parentBuffer[256];
                            parentItem.hItem = hParent;
                            parentItem.mask = TVIF_TEXT;
                            parentItem.pszText = parentBuffer;
                            parentItem.cchTextMax = sizeof(parentBuffer) / sizeof(parentBuffer[0]);
                            if (TreeView_GetItem(hTreeView, &parentItem)) {
                                parentName = parentBuffer;
                            }
                        }
                        
                        // Si el padre es Secuencial o Binaria, establecer el modo
                        if (parentName.find(L"Secuencial") != std::wstring::npos) {
                            currentMode = MODE_SEQUENTIAL;
                        } else if (parentName.find(L"Binaria") != std::wstring::npos) {
                            currentMode = MODE_BINARY;
                        }
                        
                        // Ejecutar acción según el nodo seleccionado
                        if (sel == L"Secuencial") {
                            currentMode = MODE_SEQUENTIAL;
                            if (currentArray.created) UpdateArrayListDisplay();
                            else SetWindowTextW(hTextDisplay, L"Búsqueda Secuencial\n\nNo hay arreglo creado aún.");
                        } else if (sel == L"Binaria") {
                            currentMode = MODE_BINARY;
                            if (currentArray.created) UpdateArrayListDisplay();
                            else SetWindowTextW(hTextDisplay, L"Búsqueda Binaria\n\nNo hay arreglo creado aún.");
                        } else if (sel == L"Crear") {
                            DoCreateArray();
                        } else if (sel == L"Insertar") {
                            DoInsert();
                        } else if (sel == L"Buscar") {
                            DoSearch();
                        } else if (sel == L"Eliminar") {
                            DoDelete();
                        } else if (sel == L"Guardar") {
                            DoSave();
                        } else if (sel == L"Recuperar") {
                            DoLoad();
                        } else {
                            // Para otros nodos, simplemente mostrar el nombre
                            SetWindowTextW(hTextDisplay, sel.c_str());
                        }
                        
                        // Forzar redraw del área de detalles
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                }
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            switch (id) {
                case ID_BTN_CREATE: DoCreateArray(); break;
                case ID_BTN_INSERT: DoInsert(); break;
                case ID_BTN_SEARCH: DoSearch(); break;
                case ID_BTN_DELETE: DoDelete(); break;
                case ID_BTN_SAVE: DoSave(); break;
                case ID_BTN_LOAD: DoLoad(); break;
                case ID_BTN_PRINT: MessageBoxW(hwnd, L"Función de imprimir no implementada aún.", L"Imprimir", MB_OK); break;
            }
            return 0;
        }

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void InitializeTreeView(HWND hwnd)
{
    // Crear el control TreeView
    hTreeView = CreateWindowW(
        WC_TREEVIEWW,
        L"",
        WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT,
        10, 10,
        380, 660,
        hwnd,
        (HMENU)IDC_TREEVIEW,
        GetModuleHandleW(NULL),
        NULL
    );

    // Crear el área de texto
    hTextDisplay = CreateWindowW(
        L"EDIT",
        L"Selecciona un tema para ver detalles",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        400, 10,
        780, 660,
        hwnd,
        (HMENU)IDC_RICHTEXT,
        GetModuleHandleW(NULL),
        NULL
    );

    // Configurar fuente
    // Crear fuente con tamaño legible (altura negativa = tamaño de carácter)
    HFONT hFont = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    SendMessageW(hTreeView, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hTextDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Crear botones (inicialmente ocultos)
    hBtnCreate = CreateWindowW(L"BUTTON", L"Crear", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_CREATE, GetModuleHandleW(NULL), NULL);
    hBtnInsert = CreateWindowW(L"BUTTON", L"Insertar", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_INSERT, GetModuleHandleW(NULL), NULL);
    hBtnSave = CreateWindowW(L"BUTTON", L"Guardar", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_SAVE, GetModuleHandleW(NULL), NULL);
    hBtnLoad = CreateWindowW(L"BUTTON", L"Recuperar", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_LOAD, GetModuleHandleW(NULL), NULL);
    hBtnPrint = CreateWindowW(L"BUTTON", L"Imprimir", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_PRINT, GetModuleHandleW(NULL), NULL);
    hBtnSearch = CreateWindowW(L"BUTTON", L"Buscar", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_SEARCH, GetModuleHandleW(NULL), NULL);
    hBtnDelete = CreateWindowW(L"BUTTON", L"Eliminar", WS_CHILD | WS_DISABLED,
        0,0,80,26, hwnd, (HMENU)ID_BTN_DELETE, GetModuleHandleW(NULL), NULL);

    // Lista para mostrar arreglo
    hListArray = CreateWindowW(L"LISTBOX", NULL, WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
        0,0,200,200, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    ShowWindow(hListArray, SW_HIDE);

    // Controles inline para acciones (ocultos)
    hActionLabel1 = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 0,0,300,20, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    hActionEdit1 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 0,0,200,24, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    hActionLabel2 = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE, 0,0,300,20, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    hActionEdit2 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_BORDER | ES_LEFT, 0,0,200,24, hwnd, NULL, GetModuleHandleW(NULL), NULL);
    hActionOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 0,0,60,26, hwnd, (HMENU)3001, GetModuleHandleW(NULL), NULL);
    hActionCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 0,0,80,26, hwnd, (HMENU)3002, GetModuleHandleW(NULL), NULL);
    ShowWindow(hActionLabel1, SW_HIDE); ShowWindow(hActionEdit1, SW_HIDE); ShowWindow(hActionLabel2, SW_HIDE); ShowWindow(hActionEdit2, SW_HIDE); ShowWindow(hActionOk, SW_HIDE); ShowWindow(hActionCancel, SW_HIDE);

    // Llenar el árbol
    PopulateTreeView();
}

HTREEITEM AddTreeItem(HWND hwndTV, HTREEITEM hParent, const std::wstring& text)
{
    TVINSERTSTRUCTW tvis;
    TVITEMW item;

    // Guardar texto en vector global para mantener vivo el c_str()
    g_treeStrings.push_back(text);
    std::wstring& stored = g_treeStrings.back();

    // Inicializar la estructura TVITEM
    ZeroMemory(&item, sizeof(TVITEMW));
    item.mask = TVIF_TEXT;
    item.pszText = (LPWSTR)stored.c_str();

    // Inicializar la estructura TVINSERTSTRUCTW
    ZeroMemory(&tvis, sizeof(TVINSERTSTRUCTW));
    tvis.hParent = hParent;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item = item;

    return TreeView_InsertItem(hwndTV, &tvis);
}

void PopulateTreeView()
{
    // Raiz principal
    HTREEITEM hRoot = AddTreeItem(hTreeView, NULL, L"ALGORITMOS DE BUSQUEDA");

    // Busquedas internas y externas
    HTREEITEM hBusquedas = AddTreeItem(hTreeView, hRoot, L"Busquedas internas y externas");
    
    // Secuencial con sus operaciones
    HTREEITEM hSecuencial = AddTreeItem(hTreeView, hBusquedas, L"Secuencial");
    AddTreeItem(hTreeView, hSecuencial, L"Crear");
    AddTreeItem(hTreeView, hSecuencial, L"Insertar");
    AddTreeItem(hTreeView, hSecuencial, L"Buscar");
    AddTreeItem(hTreeView, hSecuencial, L"Eliminar");
    AddTreeItem(hTreeView, hSecuencial, L"Guardar");
    AddTreeItem(hTreeView, hSecuencial, L"Recuperar");
    
    // Binaria con sus operaciones
    HTREEITEM hBinaria = AddTreeItem(hTreeView, hBusquedas, L"Binaria");
    AddTreeItem(hTreeView, hBinaria, L"Crear");
    AddTreeItem(hTreeView, hBinaria, L"Insertar");
    AddTreeItem(hTreeView, hBinaria, L"Buscar");
    AddTreeItem(hTreeView, hBinaria, L"Eliminar");
    AddTreeItem(hTreeView, hBinaria, L"Guardar");
    AddTreeItem(hTreeView, hBinaria, L"Recuperar");
    
    AddTreeItem(hTreeView, hBusquedas, L"Por transformacion de claves");
    AddTreeItem(hTreeView, hBusquedas, L"Algoritmos");
    AddTreeItem(hTreeView, hBusquedas, L"Analisis de complejidad");

    // Funciones hash
    HTREEITEM hHash = AddTreeItem(hTreeView, hRoot, L"Funciones hash o de dispersion");
    AddTreeItem(hTreeView, hHash, L"Modulo");
    AddTreeItem(hTreeView, hHash, L"Cuadrado");
    AddTreeItem(hTreeView, hHash, L"Truncamiento");
    AddTreeItem(hTreeView, hHash, L"Conversion de bases");

    // Sub-opciones de hash
    HTREEITEM hHashAnalisis = AddTreeItem(hTreeView, hRoot, L"Analisis de Hash");
    AddTreeItem(hTreeView, hHashAnalisis, L"Analisis de complejidad");
    AddTreeItem(hTreeView, hHashAnalisis, L"Diferenciacion con tiempos de acceso");

    // Solucion de colisiones
    HTREEITEM hColisiones = AddTreeItem(hTreeView, hRoot, L"Solucion de colisiones");
    AddTreeItem(hTreeView, hColisiones, L"Reasignacion");
    AddTreeItem(hTreeView, hColisiones, L"Arreglos anidados");
    AddTreeItem(hTreeView, hColisiones, L"Encadenamiento secuencial");
    AddTreeItem(hTreeView, hColisiones, L"Algoritmos y analisis");

    // Otras busquedas
    HTREEITEM hOtras = AddTreeItem(hTreeView, NULL, L"OTRAS BUSQUEDAS INTERNAS");
    AddTreeItem(hTreeView, hOtras, L"Busquedas por residuos");
    AddTreeItem(hTreeView, hOtras, L"Arboles de busqueda digital");
    AddTreeItem(hTreeView, hOtras, L"Busquedas por residuos multiple");
    AddTreeItem(hTreeView, hOtras, L"Tablas de indices");
    AddTreeItem(hTreeView, hOtras, L"Metodo de la rejilla");
    AddTreeItem(hTreeView, hOtras, L"Arboles 2D");
    AddTreeItem(hTreeView, hOtras, L"Algoritmos y analisis");

    // Indices para archivos
    HTREEITEM hIndices = AddTreeItem(hTreeView, NULL, L"INDICES PARA ARCHIVOS");
    AddTreeItem(hTreeView, hIndices, L"Definicion");
    AddTreeItem(hTreeView, hIndices, L"Indices primarios");
    AddTreeItem(hTreeView, hIndices, L"Indices secundarios");
    AddTreeItem(hTreeView, hIndices, L"Indices multiniveles");
    AddTreeItem(hTreeView, hIndices, L"Calculo de accesos a disco");
    AddTreeItem(hTreeView, hIndices, L"Algoritmos y analisis");

    // Expandir la raiz por defecto
    TreeView_Expand(hTreeView, hRoot, TVE_EXPAND);
    TreeView_Expand(hTreeView, hBusquedas, TVE_EXPAND);
}

// ----------------- Implementacion de helpers -----------------

// Mostrar/ocultar y posicionar controles de detalle
void ShowControlsForMode(Mode mode) {
    RECT rc;
    GetClientRect(hMainWindow, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int treeWidth = (width * 40) / 100;
    int x0 = treeWidth + 20;
    int y0 = 20;

    // Texto explicativo
    if (mode == MODE_SEQUENTIAL) {
        SetWindowTextW(hTextDisplay, L"Busqueda Secuencial:\r\nRecorre el arreglo elemento por elemento hasta encontrar la clave.\r\n\r\nOperaciones disponibles abajo.");
    } else if (mode == MODE_BINARY) {
        SetWindowTextW(hTextDisplay, L"Busqueda Binaria:\r\nOpera sobre arreglos ordenados dividiendo el rango y comparando la clave central.\r\n\r\nOperaciones disponibles abajo.");
    }

    // Mostrar botones en una fila bien visible
    int btn_y = y0 + 120;
    int btn_w = 85;
    int btn_h = 30;
    
    MoveWindow(hBtnCreate, x0, btn_y, btn_w, btn_h, TRUE); ShowWindow(hBtnCreate, SW_SHOW); EnableWindow(hBtnCreate, TRUE);
    MoveWindow(hBtnInsert, x0 + btn_w + 5, btn_y, btn_w, btn_h, TRUE); ShowWindow(hBtnInsert, SW_SHOW); EnableWindow(hBtnInsert, TRUE);
    MoveWindow(hBtnSearch, x0 + 2*(btn_w+5), btn_y, btn_w, btn_h, TRUE); ShowWindow(hBtnSearch, SW_SHOW); EnableWindow(hBtnSearch, TRUE);
    MoveWindow(hBtnDelete, x0 + 3*(btn_w+5), btn_y, btn_w, btn_h, TRUE); ShowWindow(hBtnDelete, SW_SHOW); EnableWindow(hBtnDelete, TRUE);
    MoveWindow(hBtnSave, x0, btn_y + btn_h + 5, btn_w, btn_h, TRUE); ShowWindow(hBtnSave, SW_SHOW); EnableWindow(hBtnSave, TRUE);
    MoveWindow(hBtnLoad, x0 + btn_w + 5, btn_y + btn_h + 5, btn_w, btn_h, TRUE); ShowWindow(hBtnLoad, SW_SHOW); EnableWindow(hBtnLoad, TRUE);
    MoveWindow(hBtnPrint, x0 + 2*(btn_w+5), btn_y + btn_h + 5, btn_w, btn_h, TRUE); ShowWindow(hBtnPrint, SW_SHOW); EnableWindow(hBtnPrint, TRUE);

    // No mostrar lista para evitar ocluir botones
    ShowWindow(hListArray, SW_HIDE);
}

void HideDetailControls() {
    ShowWindow(hBtnCreate, SW_HIDE);
    ShowWindow(hBtnInsert, SW_HIDE);
    ShowWindow(hBtnSave, SW_HIDE);
    ShowWindow(hBtnLoad, SW_HIDE);
    ShowWindow(hBtnPrint, SW_HIDE);
    ShowWindow(hBtnSearch, SW_HIDE);
    ShowWindow(hBtnDelete, SW_HIDE);
    ShowWindow(hListArray, SW_HIDE);
}

// Actualizar la lista que muestra el arreglo
void UpdateArrayListDisplay() {
    std::wstring display = L"Arreglo: ";
    display += L"[";
    for (size_t i = 0; i < currentArray.keys.size(); ++i) {
        display += currentArray.keys[i].empty() ? L"_" : currentArray.keys[i];
        if (i < currentArray.keys.size() - 1) display += L", ";
    }
    display += L"]";
    display += L"\n\nTamaño: " + std::to_wstring(currentArray.keys.size());
    display += L"\nDígitos por clave: " + std::to_wstring(currentArray.keyDigits);
    display += L"\nCreado: SÍ";
    SetWindowTextW(hTextDisplay, display.c_str());
}

// Prompt modal simple para pedir texto al usuario
std::wstring PromptModal(HWND hParent, const std::wstring& title, const std::wstring& prompt) {
    // Crear ventana simple
    const wchar_t* cls = L"PromptClass";
    static bool clsRegistered = false;
    if (!clsRegistered) {
        WNDCLASSW wc = {};
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpszClassName = cls;
        RegisterClassW(&wc);
        clsRegistered = true;
    }

    HWND hDlg = CreateWindowExW(0, cls, title.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 150, hParent, NULL, GetModuleHandleW(NULL), NULL);
    HWND hLbl = CreateWindowW(L"STATIC", prompt.c_str(), WS_CHILD | WS_VISIBLE, 10, 10, 380, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 10, 35, 380, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 230, 70, 70, 26, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 310, 70, 80, 26, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);

    ShowWindow(hDlg, SW_SHOW);
    SetForegroundWindow(hDlg);
    SetFocus(hEdit);

    std::wstring result;
    MSG msg;
    BOOL running = TRUE;
    while (running && GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.message == WM_COMMAND) {
            if ((HWND)msg.hwnd == hDlg) {
                int id = LOWORD(msg.wParam);
                if (id == 1) { // OK
                    int len = GetWindowTextLengthW(hEdit);
                    if (len > 0) {
                        wchar_t* buf = (wchar_t*)malloc((len+1)*sizeof(wchar_t));
                        GetWindowTextW(hEdit, buf, len+1);
                        result = buf;
                        free(buf);
                    }
                    DestroyWindow(hDlg);
                    running = FALSE;
                    break;
                } else if (id == 2) { // Cancel
                    DestroyWindow(hDlg);
                    running = FALSE;
                    break;
                }
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return result;
}

// Mostrar diálogo modal para crear arreglo
void DoCreateArray() {
    const wchar_t* cls = L"CreateArrayDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }

    HWND hDlg = CreateWindowExW(0, cls, L"Crear Arreglo", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 320, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    // Descripción
    CreateWindowW(L"STATIC", L"Crear Arreglo", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Crea un nuevo arreglo especificando el tamaño y el número de dígitos por clave.\nEn búsqueda secuencial se inserta en el primer espacio libre.\nEn búsqueda binaria se mantiene ordenado.", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    // Entrada de datos
    CreateWindowW(L"STATIC", L"Tamaño del arreglo:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit1 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Dígitos por clave:", WS_CHILD | WS_VISIBLE, 10, 135, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit2 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 135, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    // Botones
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 215, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 215, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);

    ShowWindow(hDlg, SW_SHOW);
    SetForegroundWindow(hDlg);
    SetFocus(hEdit1);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) { // OK
                    wchar_t buf1[64]={0}, buf2[64]={0};
                    GetWindowTextW(hEdit1, buf1, 64); GetWindowTextW(hEdit2, buf2, 64);
                    int n = _wtoi(buf1); int d = _wtoi(buf2);
                    if (n>0 && d>0) {
                        currentArray.keys.assign(n, L""); currentArray.keyDigits=d; currentArray.created=true; 
                        UpdateArrayListDisplay();
                        DestroyWindow(hDlg); 
                        MessageBoxW(hMainWindow, L"Arreglo creado correctamente.", L"Crear", MB_OK); 
                        return;
                    } else {
                        MessageBoxW(hDlg, L"Ingresa valores mayores a 0.", L"Error", MB_OK);
                    }
                } else if (id == 2) { // Cancel
                    DestroyWindow(hDlg); return;
                }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}

void DoInsert() {
    if (!currentArray.created) { MessageBoxW(hMainWindow, L"Primero crea el arreglo.", L"Info", MB_OK); return; }
    const wchar_t* cls = L"InsertDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }

    HWND hDlg = CreateWindowExW(0, cls, L"Insertar Clave", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 310, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Insertar Clave", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Inserta una nueva clave en el arreglo.\nEn búsqueda secuencial: se inserta en el primer espacio disponible.\nEn búsqueda binaria: se inserta manteniendo el orden.", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Clave a insertar:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 260, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 260, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);

    ShowWindow(hDlg, SW_SHOW); SetForegroundWindow(hDlg); SetFocus(hEdit);
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) { // OK
                    wchar_t buf[256]={0}; GetWindowTextW(hEdit, buf, 256);
                    std::wstring key(buf);
                    if ((int)key.size()!=currentArray.keyDigits) { 
                        std::wstring msg = L"La clave debe tener exactamente " + std::to_wstring(currentArray.keyDigits) + L" dígitos.";
                        MessageBoxW(hDlg, msg.c_str(), L"Error", MB_OK); 
                        continue; 
                    }
                    bool okDigits=true; for (wchar_t c: key) if (!iswdigit(c)) okDigits=false;
                    if (!okDigits) { MessageBoxW(hDlg, L"La clave debe contener solo dígitos.", L"Error", MB_OK); continue; }
                    if (currentMode==MODE_SEQUENTIAL) {
                        bool inserted=false; for (size_t i=0;i<currentArray.keys.size();++i) if (currentArray.keys[i].empty()) { currentArray.keys[i]=key; inserted=true; break; }
                        if (!inserted) MessageBoxW(hDlg, L"Arreglo lleno.", L"Info", MB_OK); else { DestroyWindow(hDlg); UpdateArrayListDisplay(); MessageBoxW(hMainWindow, L"Clave insertada.", L"Insertar", MB_OK); return; }
                    } else if (currentMode==MODE_BINARY) {
                        size_t pos=0; while (pos<currentArray.keys.size() && !currentArray.keys[pos].empty() && currentArray.keys[pos]<key) ++pos;
                        if (currentArray.keys.back().empty()) {
                            for (size_t j=currentArray.keys.size()-1;j>pos;--j) currentArray.keys[j]=currentArray.keys[j-1];
                            currentArray.keys[pos]=key; DestroyWindow(hDlg); UpdateArrayListDisplay(); MessageBoxW(hMainWindow, L"Clave insertada.", L"Insertar", MB_OK); return;
                        } else MessageBoxW(hDlg, L"Arreglo lleno.", L"Info", MB_OK);
                    }
                } else if (id == 2) { DestroyWindow(hDlg); return; }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}

void DoSearch() {
    if (!currentArray.created) { MessageBoxW(hMainWindow, L"Primero crea el arreglo.", L"Info", MB_OK); return; }
    const wchar_t* cls = L"SearchDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }

    HWND hDlg = CreateWindowExW(0, cls, L"Buscar Clave", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 310, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Buscar Clave", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Busca una clave en el arreglo usando el método seleccionado.\nEn búsqueda secuencial: recorre desde el inicio.\nEn búsqueda binaria: divide el rango de búsqueda.", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Clave a buscar:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 260, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 260, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);

    ShowWindow(hDlg, SW_SHOW); SetForegroundWindow(hDlg); SetFocus(hEdit);
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) { // OK
                    wchar_t buf[256]={0}; GetWindowTextW(hEdit, buf, 256);
                    std::wstring key(buf);
                    if ((int)key.size()!=currentArray.keyDigits) { MessageBoxW(hDlg, L"Tamaño de clave inválido.", L"Error", MB_OK); continue; }
                    if (currentMode==MODE_SEQUENTIAL) {
                        for (size_t i=0;i<currentArray.keys.size();++i) if (currentArray.keys[i]==key) { MessageBoxW(hDlg, (L"Encontrado en índice "+std::to_wstring(i)).c_str(), L"Resultado", MB_OK); DestroyWindow(hDlg); return; }
                        MessageBoxW(hDlg, L"No encontrado.", L"Resultado", MB_OK);
                    } else {
                        std::vector<std::wstring> elems;
                        for (auto &k: currentArray.keys) if (!k.empty()) elems.push_back(k);
                        size_t lo=0, hi=(elems.empty()?0:elems.size()-1);
                        bool found=false;
                        size_t idx=0;
                        while (!elems.empty() && lo<=hi) {
                            size_t mid=(lo+hi)/2;
                            if (elems[mid]==key) {
                                found=true;
                                idx=mid;
                                break;
                            } else if (elems[mid]<key) {
                                lo=mid+1;
                            } else if (mid==0) {
                                break;
                            } else {
                                hi=mid-1;
                            }
                        }
                        if (found) {
                            std::wstring resultMsg = L"Encontrado (posición: " + std::to_wstring(idx) + L")";
                            MessageBoxW(hDlg, resultMsg.c_str(), L"Resultado", MB_OK);
                        }
                        else MessageBoxW(hDlg, L"No encontrado.", L"Resultado", MB_OK);
                    }
                    DestroyWindow(hDlg); return;
                } else if (id == 2) { DestroyWindow(hDlg); return; }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}

void DoDelete() {
    if (!currentArray.created) { MessageBoxW(hMainWindow, L"Primero crea el arreglo.", L"Info", MB_OK); return; }
    const wchar_t* cls = L"DeleteDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }

    HWND hDlg = CreateWindowExW(0, cls, L"Eliminar Clave", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 310, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Eliminar Clave", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Elimina una clave del arreglo.\nLa posición quedará vacía (marcada con '_').\nPara mantener orden en búsqueda binaria, considera usar Guardar/Recuperar.", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Clave a eliminar:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 260, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 260, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);

    ShowWindow(hDlg, SW_SHOW); SetForegroundWindow(hDlg); SetFocus(hEdit);
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) { // OK
                    wchar_t buf[256]={0}; GetWindowTextW(hEdit, buf, 256);
                    std::wstring key(buf);
                    for (size_t i=0;i<currentArray.keys.size();++i) {
                        if (currentArray.keys[i]==key) { currentArray.keys[i]=L""; UpdateArrayListDisplay(); DestroyWindow(hDlg); MessageBoxW(hMainWindow, L"Clave eliminada.", L"Eliminar", MB_OK); return; }
                    }
                    MessageBoxW(hDlg, L"Clave no encontrada.", L"Eliminar", MB_OK); DestroyWindow(hDlg); return;
                } else if (id == 2) { DestroyWindow(hDlg); return; }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}

void DoSave() {
    if (!currentArray.created) { MessageBoxW(hMainWindow, L"No hay arreglo para guardar.", L"Info", MB_OK); return; }
    const wchar_t* cls = L"SaveDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }
    
    HWND hDlg = CreateWindowExW(0, cls, L"Guardar Arreglo", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 310, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Guardar Arreglo", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Guarda el arreglo actual en un archivo.\nSe guardará en formato: [dígitos] [tamaño]\\nclave1\\nclave2\\n...", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Nombre de archivo:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"datos.txt", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 260, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 260, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);
    
    ShowWindow(hDlg, SW_SHOW); SetForegroundWindow(hDlg); SetFocus(hEdit);
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) {
                    wchar_t buf[256]={0}; GetWindowTextW(hEdit, buf, 256);
                    std::wstring fname(buf);
                    if (fname.empty()) { MessageBoxW(hDlg, L"Nombre de archivo vacío.", L"Error", MB_OK); continue; }
                    std::string sname(fname.begin(), fname.end());
                    std::ofstream ofs(sname);
                    if (!ofs) { MessageBoxW(hDlg, L"No se pudo crear el archivo.", L"Error", MB_OK); continue; }
                    ofs << currentArray.keyDigits << " " << currentArray.keys.size() << "\n";
                    for (auto &k : currentArray.keys) { std::string sk(k.begin(), k.end()); ofs << sk << "\n"; }
                    ofs.close();
                    std::wstring msg = L"Guardado en: " + fname;
                    DestroyWindow(hDlg); MessageBoxW(hMainWindow, msg.c_str(), L"Guardar", MB_OK); return;
                } else if (id == 2) { DestroyWindow(hDlg); return; }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}

void DoLoad() {
    const wchar_t* cls = L"LoadDlg";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc = {}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = cls;
        RegisterClassW(&wc); registered = true;
    }
    
    HWND hDlg = CreateWindowExW(0, cls, L"Recuperar Arreglo", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 310, hMainWindow, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Recuperar Arreglo", WS_CHILD | WS_VISIBLE, 10, 10, 470, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    CreateWindowW(L"STATIC", L"Carga un arreglo previamente guardado.\nEl archivo debe estar en el formato de guardado.\nReemplazará el arreglo actual.", 
        WS_CHILD | WS_VISIBLE, 10, 35, 470, 50, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    CreateWindowW(L"STATIC", L"Nombre de archivo:", WS_CHILD | WS_VISIBLE, 10, 95, 200, 20, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    HWND hEdit = CreateWindowW(L"EDIT", L"datos.txt", WS_CHILD | WS_VISIBLE | WS_BORDER, 220, 95, 150, 24, hDlg, NULL, GetModuleHandleW(NULL), NULL);
    
    HWND hOk = CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE, 200, 260, 80, 28, hDlg, (HMENU)1, GetModuleHandleW(NULL), NULL);
    HWND hCancel = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE, 290, 260, 80, 28, hDlg, (HMENU)2, GetModuleHandleW(NULL), NULL);
    
    ShowWindow(hDlg, SW_SHOW); SetForegroundWindow(hDlg); SetFocus(hEdit);
    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        if (msg.hwnd == hDlg || IsChild(hDlg, msg.hwnd)) {
            if (msg.message == WM_COMMAND) {
                int id = LOWORD(msg.wParam);
                if (id == 1) {
                    wchar_t buf[256]={0}; GetWindowTextW(hEdit, buf, 256);
                    std::wstring fname(buf);
                    if (fname.empty()) { MessageBoxW(hDlg, L"Nombre de archivo vacío.", L"Error", MB_OK); continue; }
                    std::string sname(fname.begin(), fname.end());
                    std::ifstream ifs(sname);
                    if (!ifs) { MessageBoxW(hDlg, L"No se pudo abrir el archivo.", L"Error", MB_OK); continue; }
                    int digits=0, n=0; ifs >> digits >> n; std::string line; std::getline(ifs, line);
                    currentArray.keys.assign(n, L""); currentArray.keyDigits = digits; currentArray.created = true;
                    for (int i=0;i<n && std::getline(ifs,line);++i) { currentArray.keys[i]=std::wstring(line.begin(), line.end()); }
                    ifs.close();
                    DestroyWindow(hDlg); UpdateArrayListDisplay(); MessageBoxW(hMainWindow, L"Arreglo cargado exitosamente.", L"Recuperar", MB_OK); return;
                } else if (id == 2) { DestroyWindow(hDlg); return; }
            }
        }
        TranslateMessage(&msg); DispatchMessageW(&msg);
    }
}
