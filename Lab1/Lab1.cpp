// Lab1.cpp : Определяет точку входа для приложения.
//
#pragma warning (disable : 4996)

#include "framework.h"
#include "Lab1.h"
#include "stdio.h"
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include "shlwapi.h"

#define MAX_LOADSTRING 100
#define MaxLength 256

//Структура данных, которые используются в программе
struct information
{
    wchar_t *name = new wchar_t[MaxLength];
    wchar_t *CPU = new wchar_t[MaxLength];
    float memory;
    int cost;
};

//Функция, определяющая, какой из элементов в массиве меньше (определяет порядок сортировки в методе sort()
bool InfoSort(information first, information second)
{
    return first.cost < second.cost;
}

//Тип перечисления. В данной программе используются для работы с поиском. CPUFlags отвечает за поиск процессоров, RAMFlags - объём памяти
enum class CPUFlags
{
    not_selected,
    intel_core_i5,
    intel_core_i7,
    AMD,
    others
};

enum class RAMFlags
{
    not_selected,
    GB1,
    GB2,
    GB4,
    GB8,
    GB16,
    others
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
FILE *fileStream;                               // файловый поток
std::vector<information> fileInfo;              //динамический список с информацией о компьютерах
HWND hList1;
RAMFlags RAMFind = (RAMFlags)0;                 // флаг радио-кнопок поиска по объёму памяти
wchar_t* RAMName = new wchar_t[MaxLength];      // строка, содержащая текст из поля "Другое" диалогового окна поиска по объёму памяти
CPUFlags CPUFind = (CPUFlags)0;                 // флаг радио-кнопок поиска по процессору
wchar_t* CPUName = new wchar_t[MaxLength];      // строка, содержащая текст из поля "Другое" диалогового окна поиска по процессору
HBITMAP hBall;                                  // дескриптор .bmp файла

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    DlgProcAdd(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProcSearchCPU(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    DlgProcSearchRAM(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    AnimProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable1 = LoadAccelerators(hInstance, MAKEINTRESOURCE(130)); // кастомная таблица горячих клавиш
    HACCEL hAccelTable2 = LoadAccelerators(hInstance, MAKEINTRESOURCE(109));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable1, &msg))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable2, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_MYPOINTER));
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON2));

    return RegisterClassExW(&wcex);
}

/*HWND CreateListViewPC(HWND hwndParent)
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    RECT rcClient;                       // The parent window's client area.

    GetClientRect(hwndParent, &rcClient);

    // Create the list-view window in report view with label editing enabled.
    HWND hWndListView = CreateWindow(WC_LISTVIEW,
        L"",
        WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
        0, 0,
        rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top,
        hwndParent,
        NULL,
        hInst,
        NULL);

    return (hWndListView);
}*/

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной
   HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   SetMenu(hWnd, hMenu);
   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//

void HandleWM_NOTIFY(LPARAM lParam)
{
    NMLVDISPINFO* plvdi;

    switch (((LPNMHDR)lParam)->code)
    {
    case LVN_GETDISPINFO:

        plvdi = (NMLVDISPINFO*)lParam;

        switch (plvdi->item.iSubItem)
        {
        case 0:
            plvdi->item.pszText = fileInfo[plvdi->item.iItem].name;
            break;

        case 1:
            plvdi->item.pszText = fileInfo[plvdi->item.iItem].CPU;
            break;

        case 2:
            wsprintf(plvdi->item.pszText, L"%f", fileInfo[plvdi->item.iItem].memory);
            break;
        case 3:
            wsprintf(plvdi->item.pszText, L"%d", fileInfo[plvdi->item.iItem].cost);
            break;
        default:
            break;
        }

        break;

    }
    // NOTE: In addition to setting pszText to point to the item text, you could 
    // copy the item text into pszText using StringCchCopy. For example:
    //
    // StringCchCopy(plvdi->item.pszText, 
    //                         plvdi->item.cchTextMax, 
    //                         rgPetInfo[plvdi->item.iItem].szKind);

    return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HMENU hMenu, hSubMenu;              // дескрипторы меню и подменю
    POINT point;                        // координаты
    int xpos, ypos, wmId, wmEvent;      // координаты (xpos, ypos), идентификатор команды(wmId) и события (wmEvent)
    HDC hdc;                            // дескриптор экрана
    PAINTSTRUCT ps;                     // структура рисования
    static OPENFILENAME file;           // структура, содержащая параметры открытия файла
    static TCHAR name[MAX_PATH];
    static HANDLE hFile;                // дескриптор файла
    BOOL success;                       
    int j = 0;
    DWORD dw = 0;
    wchar_t text[MaxLength];            // строка считанного текста из файла
    char filename[MAX_PATH];            // имя файла
    switch (message)
    {
    case WM_CREATE:
        file.lStructSize = sizeof(OPENFILENAME);
        file.hInstance = hInst;
        file.lpstrFilter = _T("Text\0*.txt");
        file.lpstrFile = name;
        file.nMaxFile = 256;
        file.lpstrInitialDir = _T(".\\");
        file.lpstrDefExt = _T("txt");
        hBall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3)); // загрузка .bmp файла
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case ID_FILE_OPEN:
                file.lpstrTitle = _T("Открыть файл для чтения");
                file.Flags = OFN_HIDEREADONLY;
                if (!GetOpenFileName(&file)) return 1; // получение имени файла
                wcstombs(filename, file.lpstrFile, MAX_PATH);
                if ((fileStream = fopen(filename, "r")) != NULL) // если удалось открыть файл для чтения, читаем
                {
                    fileInfo.clear(); // очистка списка от старых записей
                    information newElem;
                    while (fgetws(text, MaxLength, fileStream) != NULL) // пока не встретили EOF (конец файла)
                    {
                        // Инициализируем новый элемент
                        int index = 0;
                        newElem.name = new wchar_t[MaxLength];
                        newElem.CPU = new wchar_t[MaxLength];
                        newElem.memory = 0;
                        newElem.cost = 0;
                        // Находим все места вхождения сепараторов (в нашем случае '#')
                        int markIndex = 0;
                        int mark[4] = { 0, 0, 0, 0 };
                        while (text[index] != L'\n' && text[index] != L'\0')
                        {
                            if (text[index] != L'#')
                            {
                                mark[markIndex]++;
                            }
                            else
                            {
                                markIndex++;
                            }
                            index++;
                        }
                        text[index] = L'\0';
                        /*
                        wchar_t *newElem = new wchar_t[MaxLength];
                        wmemcpy(newElem, text, MaxLength);
                        fileInfo.push_back(newElem);
                        */
                        // Парсим строку на отдельные элементы и записываем их в соответствующие поля нового элемента
                        markIndex = 0;
                        wchar_t* pred;
                        wchar_t* cutName;
                        wchar_t* cutCPU;
                        wchar_t* cutMemory;
                        // Имя компьютера - строка
                        wchar_t* part = new wchar_t[MaxLength];
                        memccpy(part, text, L'#', MaxLength);
                        pred = wmemchr(text, L'#', MaxLength);
                        cutName = wmemchr(pred, pred[1], MaxLength);
                        part[mark[markIndex]] = L'\0';
                        markIndex++;
                        wmemcpy(newElem.name, part, MaxLength);
                        // Процессор - строка
                        part = new wchar_t[MaxLength];
                        memccpy(part, cutName, L'#', MaxLength);
                        pred = wmemchr(cutName, L'#', MaxLength);
                        cutCPU = wmemchr(pred, pred[1], MaxLength);
                        part[mark[markIndex]] = L'\0';
                        wmemcpy(newElem.CPU, part, MaxLength);
                        // Объём памяти - число с плавающей точкой
                        part = new wchar_t[MaxLength];
                        memccpy(part, cutCPU, L'#', MaxLength);
                        pred = wmemchr(cutCPU, L'#', MaxLength);
                        cutMemory = wmemchr(pred, pred[1], MaxLength);
                        index = 0;
                        float multiply = 0.1;
                        bool decimalPoint = false;
                        // Ручной перевод строки в число с плавающей точкой до первого не числового символа
                        while ((part[index] <= L'9' && part[index] >= L'0') || part[index] == L'.')
                        {
                            if (part[index] == L'.')
                            {
                                decimalPoint = !decimalPoint;
                            }
                            else if (!decimalPoint)
                            {
                                newElem.memory = (newElem.memory * 10) + (part[index] - L'0');
                            }
                            else
                            {
                                newElem.memory = newElem.memory + ((part[index] - L'0') * multiply);
                                multiply *= 0.1;
                            }
                            index++;
                        }
                        index = 0;
                        // Остаток строки является ценой - целым числом. Ручной перевод из строки в целое число до первого нечислового символа
                        while (cutMemory[index] <= L'9' && cutMemory[index] >= L'0')
                        {
                            newElem.cost = (newElem.cost * 10) + cutMemory[index] - L'0';
                            index++;
                        }
                        fileInfo.push_back(newElem);        // добавление элемента в конец списка
                    }
                    // Объявляем недействительным изображение в окне и запрашиваем перерисовку
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
                if (fileStream) fclose(fileStream);     // Закрываем поток!!!
                break;
            case ID_FILE_SAVE:
                file.lpstrTitle = _T("Сохранить файл как...");
                file.Flags = OFN_HIDEREADONLY;
                if (!GetOpenFileName(&file)) return 1; // Получение имени файла
                wcstombs(filename, file.lpstrFile, MAX_PATH);
                if ((fileStream = fopen(filename, "w")) != NULL) // Если открыли для записи файл, то записываем
                {
                    for (int index = 0; index < fileInfo.size(); index++)
                    {
                        fwprintf(fileStream,L"%s#%s#%0.1f#%d\n",fileInfo[index].name,fileInfo[index].CPU,fileInfo[index].memory,fileInfo[index].cost); // записываем в том же формате, что и читаем!!!
                    }
                    fileInfo.clear(); // Очищаем список от записаных данных
                }
                if (fileStream) fclose(fileStream); // Освобождаем поток!!!
                break;
            case ID_INFO_ADD:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD), hWnd, DlgProcAdd); // Вызываем диалоговое окно IDD_ADD, сообщения которого будет обрабатывать процедура DlgProcAdd
                // Объявляем недействительным изображение в окне и запрашиваем перерисовку
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
                break;
            case ID_INFO_CPU_TYPE:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_CPU), hWnd, DlgProcSearchCPU);  // Вызываем диалоговое окно IDD_SEARCH_CPU, сообщения которого будет обрабатывать процедура DlgProcSearchCPU
                // Объявляем недействительным изображение в окне и запрашиваем перерисовку
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
                break;
            case ID_INFO_RAM:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_RAM), hWnd, DlgProcSearchRAM);  // Вызываем диалоговое окно IDD_SEARCH_RAM, сообщения которого будет обрабатывать процедура DlgProcSearchRAM
                // Объявляем недействительным изображение в окне и запрашиваем перерисовку
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
                break;
            case ID_INFO_RESET:
                // Сбрасываем параметры поиска
                CPUFind = CPUFlags::not_selected;
                RAMFind = RAMFlags::not_selected;
                // Объявляем недействительным изображение в окне и запрашиваем перерисовку
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
                break;
            case IDM_ABOUT:
            case ID_HELP_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_ANIM:
                // Вызываем окно с анимацией
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ANIM), hWnd, AnimProc);
                break;
            case IDM_EXIT:
            case ID_FILE_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    // Отрисовываем основное окно
    case WM_PAINT:
        {            
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (fileInfo.size() != 0)
            {
                std::vector<information> list; // конечный список, который надо отобразить
                for (int index = 0; index < fileInfo.size(); index++)
                {
                    bool addFlag = false; // добавили ли мы текущий элемент в конечный список для отображения
                    wchar_t* ram = new wchar_t[MaxLength];
                    _snwprintf(ram, MaxLength, L"%0.1f\0", fileInfo[index].memory);
                    // Проверяем, подходит ли элемент по первому критерию. Если подходит - добавляем
                    switch (RAMFind)
                    {
                    case RAMFlags::GB1:
                    {
                        if (fabs(fileInfo[index].memory - 1.0) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    case RAMFlags::GB2:
                    {
                        if (fabs(fileInfo[index].memory - 2.0) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    case RAMFlags::GB4:
                    {
                        if (fabs(fileInfo[index].memory - 4.0) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    case RAMFlags::GB8:
                    {
                        if (fabs(fileInfo[index].memory - 8.0) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    case RAMFlags::GB16:
                    {
                        if (fabs(fileInfo[index].memory - 16.0) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    case RAMFlags::not_selected:
                    {
                        list.push_back(fileInfo[index]);
                        addFlag = true;
                        break;
                    }
                    default:
                    {
                        float RAM = 0;
                        int indexRam = 0;
                        float multiply = 0.1;
                        bool decimalPoint = false;
                        while ((RAMName[indexRam] <= L'9' && RAMName[indexRam] >= L'0') || RAMName[indexRam] == L'.')
                        {
                            if (RAMName[indexRam] == L'.')
                            {
                                decimalPoint = !decimalPoint;
                            }
                            else if (!decimalPoint)
                            {
                                RAM = (RAM * 10) + (RAMName[indexRam] - L'0');
                            }
                            else
                            {
                                RAM = RAM + ((RAMName[indexRam] - L'0') * multiply);
                                multiply *= 0.1;
                            }
                            indexRam++;
                        }
                        if (fabs(fileInfo[index].memory - RAM) < 0.01) // сравнение чисел с плавающей точкой необходимо делать через модуль разности двух чисел
                        {
                            list.push_back(fileInfo[index]);
                            addFlag = true;
                        }
                        break;
                    }
                    }
                    if (addFlag) // если по предыдущему критерию элемент подходит для отображения
                    {
                        // Проверяем второй критерий. Если не подходит - убираем
                        switch (CPUFind)
                        {
                        case CPUFlags::intel_core_i5:
                        {
                            if (wcsstr(fileInfo[index].CPU, L"Intel Core i-5") == NULL)
                            {
                                list.pop_back();
                            }
                            break;
                        }
                        case CPUFlags::intel_core_i7:
                        {
                            if (wcsstr(fileInfo[index].CPU, L"Intel Core i-7") == NULL)
                            {
                                list.pop_back();
                            }
                            break;
                        }
                        case CPUFlags::AMD:
                        {
                            if (wcsstr(fileInfo[index].CPU, L"AMD") == NULL)
                            {
                                list.pop_back();
                            }
                            break;
                        }
                        case CPUFlags::not_selected:
                        {
                            break;
                        }
                        default:
                        {
                            if (wcsstr(fileInfo[index].CPU, CPUName) == NULL)
                            {
                                list.pop_back();
                            }
                            break;
                        }
                        }
                    }
                }
                j = 0;
                std::sort(list.begin(), list.end(), InfoSort); // Сортируем список. Указываем на начало и конец списка, а также сообщаем, какой процедурой сравнивать элементы
                // Выводим на экран в виде текста информацию из списка, прошедшие через фильтрацию по процессору и объёму памяти
                for (int index = 0; index < list.size(); index++)
                {
                    wchar_t* textOut = new wchar_t[MaxLength];
                    int length = _snwprintf(textOut, MaxLength, L"Model: %s; CPU: %s; RAM: %0.1f; Cost: %d\0", list[index].name, list[index].CPU, list[index].memory, list[index].cost);
                    TextOutW(hdc, 0, j, textOut, length);
                    j += 16;
                }
            }
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    // Контекстное меню
    case WM_RBUTTONDOWN:
        {
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            ClientToScreen(hWnd, &point);
            xpos = point.x;
            ypos = point.y;
            hMenu = LoadMenu(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MENU2));
            hSubMenu = GetSubMenu(hMenu, 0);
            TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, xpos, ypos, 0, hWnd, NULL);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int x = 0, y = 0;
bool up = false, left = false;
RECT rect;
HDC hdcBall;

//Процедура обработки сообщений в диалоговом окне "Анимация"
INT_PTR CALLBACK AnimProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    HBRUSH hBrush; // дескриптор кисти
    switch (message)
    {
    case WM_INITDIALOG:
        hdc = GetDC(hWnd);
        hdcBall = CreateCompatibleDC(hdc); // создаём виртуальное совместимое с текущим экраном устройство
        SelectObject(hdcBall, hBall); // выбираем объект, находящийся в виртуальном устройстве
        ReleaseDC(hWnd, hdc);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_START_ANIM:
            SetTimer(hWnd, 1, 5, (TIMERPROC)NULL); // установка таймера. (дескриптор окна, идентификатор таймера, время, срабатывающая процедура). Если последний параметр NULL - обработка идёт в WM_TIMER
            break;
        case IDC_STOP_ANIM:
            KillTimer(hWnd, 1); // остановка таймера. (дескриптор окна, идентификатор таймера)
            break;
        case IDC_EXIT_ANIM:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        return (INT_PTR)TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return (INT_PTR)TRUE;
    case WM_TIMER:
        switch (wParam)
        {
        case 1:
            GetClientRect(hWnd, &rect); // координаты, описывающее окно
            rect.bottom -= 100; // отсекаем нижних 100 пикселей от окна для дальнейшей обработки
            // Проверяем, не выйдет ли мяч за пределы поля
            if (x + 64 + 1 >= rect.right && !left)
            {
                left = !left;
            }
            else if (x - 1 <= rect.left && left)
            {
                left = !left;
            }
            if (y + 64 + 1 >= rect.bottom && !up)
            {
                up = !up;
            }
            else if (y - 1 <= rect.top && up)
            {
                up = !up;
            }
            // Редактируем координаты левого верхнего угла картинки мяча
            if (up)
            {
                y--;
            }
            else
            {
                y++;
            }
            if (left)
            {
                x--;
            }
            else
            {
                x++;
            }
            hdc = GetDC(hWnd);
            hBrush = CreateSolidBrush(RGB(255, 255, 255)); // создаём кисть
            SelectObject(hdc, hBrush); // выбираем кисть
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom); //рисуем кистью прямоугольник
            BitBlt(hdc, x, y, 64, 64, hdcBall, 0, 0, SRCCOPY); // рисуем картинку с виртуального устройства в окно
            ReleaseDC(hWnd, hdc);
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

HWND hModel, hCPU, hRAM, hCost;

// Процедура обработки сообщений окна "Добавление нового ПК"
INT_PTR CALLBACK DlgProcAdd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        // Получаем дескрипторы полей ввода данных
        hModel = GetDlgItem(hWnd, IDC_ADD_MODEL);
        hCPU = GetDlgItem(hWnd, IDC_ADD_CPU);
        hRAM = GetDlgItem(hWnd, IDC_ADD_RAM);
        hCost = GetDlgItem(hWnd, IDC_ADD_COST);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_ADD_BUTTON)
        {
            // Создаём новый элемент и добавляем в список
            information newElem = { new wchar_t[MaxLength], new wchar_t[MaxLength], 0, 0 };
            GetDlgItemText(hWnd, IDC_ADD_MODEL, newElem.name, MaxLength);
            GetDlgItemText(hWnd, IDC_ADD_CPU, newElem.CPU, MaxLength);
            newElem.memory = GetDlgItemInt(hWnd, IDC_ADD_RAM, 0, 1);
            newElem.cost = GetDlgItemInt(hWnd, IDC_ADD_COST, 0, 1);
            fileInfo.push_back(newElem);
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        else if (LOWORD(wParam) == IDC_ADD_CANCEL)
        {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        return (INT_PTR)TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

HWND hText;
RAMFlags RAMFindNew = (RAMFlags)0;
CPUFlags CPUFindNew = (CPUFlags)0;

// Процедура обработки сообщений окна "Поиск по процессору"
INT_PTR CALLBACK DlgProcSearchCPU(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hText = GetDlgItem(hWnd, IDC_SEARCH_CPU_OTHER);
        CPUFindNew = CPUFind;
        SetWindowTextW(hText, CPUName);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SEARCH_CPU_AMD_RADIO:
            CPUFindNew = CPUFlags::AMD;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_CPU_I5_RADIO:
            CPUFindNew = CPUFlags::intel_core_i5;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_CPU_I7_RADIO:
            CPUFindNew = CPUFlags::intel_core_i7;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_CPU_OTHER_RADIO:
            CPUFindNew = CPUFlags::others;
            EnableWindow(hText, TRUE);
            break;
        case IDC_SEARCH_CPU_CONFIRM:
            CPUFind = CPUFindNew;
            if (CPUFind == CPUFlags::others) GetDlgItemText(hWnd, IDC_SEARCH_CPU_OTHER, CPUName, MaxLength);
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case IDC_SEARCH_CPU_CANCEL:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        return (INT_PTR)TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

//Процедура обработки сообщений окна "Поиск по объёму памяти"
INT_PTR CALLBACK DlgProcSearchRAM(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        hText = GetDlgItem(hWnd, IDC_SEARCH_RAM_OTHER);
        RAMFindNew = RAMFind;
        SetWindowTextW(hText, RAMName);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_SEARCH_1GB_RADIO:
            RAMFindNew = RAMFlags::GB1;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_2GB_RADIO:
            RAMFindNew = RAMFlags::GB2;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_4GB_RADIO:
            RAMFindNew = RAMFlags::GB4;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_8GB_RADIO:
            RAMFindNew = RAMFlags::GB8;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_16GB_RADIO:
            RAMFindNew = RAMFlags::GB16;
            EnableWindow(hText, FALSE);
            break;
        case IDC_SEARCH_RAM_OTHER_RADIO:
            RAMFindNew = RAMFlags::others;
            EnableWindow(hText, TRUE);
            break;
        case IDC_SEARCH_RAM_CONFIRM:
            RAMFind = RAMFindNew;
            if (RAMFind == RAMFlags::others) GetDlgItemText(hWnd, IDC_SEARCH_RAM_OTHER, RAMName, MaxLength);
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case IDC_SEARCH_RAM_CANCEL:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        return (INT_PTR)TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
