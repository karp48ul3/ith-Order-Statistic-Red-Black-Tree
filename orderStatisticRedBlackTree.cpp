// RBTree_desktop.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <WinUser.h>
#include "framework.h"
#include "RBTree_desktop.h"
#include <iostream>
#include <vector>
#include <optional>
#include <string>
#include <utility>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

int ORDER_STAT = 6;

// color of a node
enum class Color { Red, Black };

// define Comparable concept for values of type T to be compared to each other
template<typename T>
concept Comparable = requires(T a, T b) { a <=> b; };

// definition of tree node
template<typename T>
requires Comparable<T>
struct Node {
    Node* parent; // parent
    Node* left; //left child
    Node* right;//right child
    T key; // key
    int size; // size of a subtree rooted at Node
    ::Color color; // color of a node
};

template<typename T>
requires Comparable<T>
class RBTree { // class representing red black tree

    Node<T>* root; // root of the tree

    void TreeDestructorHelper(Node<T>* node) {
        if (node->left != nullptr) {
            TreeDestructorHelper(node->left);
        }
        if (node->right != nullptr) {
            TreeDestructorHelper(node->right);
        }
        delete node;
        node = nullptr;
    }

    void RBInsertFixup(Node<T>* z) {
        if (z->parent == nullptr) {// z is the root so return
            return;
        }
        while (z->parent != nullptr && z->parent->color == ::Color::Red) {
            // while z parent is red. Otherwise parent is black and the tree is balanced
            auto grandparent = z->parent->parent;
            auto uncle = root;
            if (z->parent == grandparent->left) { // if parent is left child of a grandparent
                auto y = z->parent->parent->right;
                if (grandparent->right != nullptr) {
                    uncle = grandparent->right;
                }


                if (uncle->color == ::Color::Red) { // case 1 .  uncle is red
                    z->parent->color = ::Color::Black;
                    uncle->color = ::Color::Black;
                    z->parent->parent->color = ::Color::Red;
                    z = z->parent->parent;
                }
                else if (z == z->parent->right) { // case 2 . uncle is black and z is right child
                    z = z->parent;
                    LeftRotate(z);
                }
                else {
                    z->parent->color = ::Color::Black;  // case 3 . uncle is black and z is left child
                    z->parent->parent->color = ::Color::Red;
                    RightRotate(z->parent->parent);
                    z = grandparent;
                }


            }
            else { // symmetric cases
                if (grandparent->left) { uncle = grandparent->left; }
                if (uncle->color == ::Color::Red) {
                    z->parent->color = ::Color::Black;
                    uncle->color = ::Color::Black;
                    grandparent->color = ::Color::Red;
                    z = grandparent;
                }
                else if (z == grandparent->right->left) {
                    z = z->parent;
                    RightRotate(z);
                }
                else {
                    z->parent->color = ::Color::Black;
                    grandparent->color = ::Color::Red;
                    LeftRotate(grandparent);
                    z = grandparent;
                }

            }
        }
        root->color = ::Color::Black; // paint the root black
    }

    void LeftRotate(Node<T>* x) {
        auto y = x->right;
        x->right = y->left;
        if (y->left != nullptr) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;

        // update size field
        y->size = x->size;
        if (x->right != nullptr && x->left != nullptr) {
            x->size = x->right->size + x->left->size + 1;
        }
        else if (x->right != nullptr) {
            x->size = x->right->size + 1;
        }
        else if (x->left != nullptr) {
            x->size = x->left->size + 1;
        }
        else {
            x->size = 1;
        }
    }

    void RightRotate(Node<T>* x) {
        auto y = x->left;
        x->left = y->right;
        if (y->right != nullptr) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
        y->right = x;
        x->parent = y;

        //update size field
        y->size = x->size;

        if (x->right != nullptr && x->left != nullptr) {
            x->size = x->right->size + x->left->size + 1;
        }
        else if (x->right != nullptr) {
            x->size = x->right->size + 1;
        }
        else if (x->left != nullptr) {
            x->size = x->left->size + 1;
        }
        else {
            x->size = 1;
        }
    }

public:
    RBTree(T rootKey) : root{ new Node<T> } { // construct with the root
        root->parent = nullptr;
        root->right = nullptr;
        root->left = nullptr;
        root->key = rootKey;
        root->size = 1;
        root->color = ::Color::Black;
    }

    ~RBTree() { // destructor
        TreeDestructorHelper(root);
    }

    auto GetRoot() {
        return root;
    }

    void RBInsert(T key) {
        //initialize Node y to be parent of x and x = root
        Node<T>* y = nullptr;
        auto x = this->root;

        //dynamically create z Node to be inserted
        Node<T>* z = new Node<T>;
        z->key = key;
        z->parent = nullptr;
        z->right = nullptr;
        z->left = nullptr;
        z->color = ::Color::Black;

        // go down the tree up to its leaf
        // same as in normal binary search tree
        while (x != nullptr) {
            y = x;
            // if z's key is less than key of node x go to x left child
            if (key < x->key) {
                x = x->left;
            }
            else {// otherwise go to x right child
                x = x->right;
            }

            y->size++;// increment size of y Node by one
        }
        z->parent = y;
        if (y == nullptr) {
            root = z;
        }
        else if (key < y->key) {
            y->left = z;
        }
        else {
            y->right = z;
        }

        // assign nullptr to z children
        z->left = nullptr;
        z->right = nullptr;
        z->color = ::Color::Red;

        // size of z is 1
        z->size = 1;

        // bring back tree colouring property
        RBInsertFixup(z);
    }

    T getOrderStatistic(int i) {
        if (i > root->size || i < 0) { // check if i is in allowed range
            std::cout << "i exceeds allowed range";
            return static_cast<T>(0);
        }
        auto currentNode = root;

        // loop down the tree from root until we find i-th smallest element
        while (currentNode != nullptr) {
            if (currentNode->left != nullptr) {
                if (i < currentNode->left->size + 1) { // if 'i' is lower than size of currentNode->left +1 
                    // go to left subtree of red black tree
                    currentNode = currentNode->left;
                }
                else if (i == currentNode->left->size + 1) { // is 'i' is equall then order statistic is currentNode
                    return currentNode->key;
                }
                else {// if 'i' is greater go to right subtree
                    // and update i (substract currentNode->left->size + 1)
                    i = i - (currentNode->size - currentNode->right->size);
                    currentNode = currentNode->right;

                }
            }
            else { // if left subtree od currentNode is empty
                if (i == 1) {
                    return currentNode->key;
                }
                else { // go to right subtree
                    i = i - (currentNode->size - currentNode->right->size); // substract from i 
                    // currentNode->left->size +1
                    currentNode = currentNode->right;
                }
            }
        }
    }
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
ULONG_PTR gdiPlusToken;

template<typename T>
requires Comparable<T>
RBTree<T> rbtree{20};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    Gdiplus::GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, nullptr);

    // insert values do red-black tree
    rbtree<int>.RBInsert(12);
    rbtree<int>.RBInsert(38);
    rbtree<int>.RBInsert(10);
    rbtree<int>.RBInsert(36);
    rbtree<int>.RBInsert(8);
    rbtree<int>.RBInsert(34);
    rbtree<int>.RBInsert(6);
    rbtree<int>.RBInsert(32);
    rbtree<int>.RBInsert(4);
    rbtree<int>.RBInsert(30);
    rbtree<int>.RBInsert(2);
    rbtree<int>.RBInsert(28);
    rbtree<int>.RBInsert(16);
    rbtree<int>.RBInsert(26);
    rbtree<int>.RBInsert(18);
    rbtree<int>.RBInsert(14);
    rbtree<int>.RBInsert(22);
    rbtree<int>.RBInsert(24);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_RBTREEDESKTOP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RBTREEDESKTOP));

    MSG msg;



    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    Gdiplus::GdiplusShutdown(gdiPlusToken);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RBTREEDESKTOP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RBTREEDESKTOP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// text box for i in ith order statistic
HWND hWndEdit = 0;
bool guard = true;

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow);

   UpdateWindow(hWnd);

   return TRUE;
}

template<typename T>
requires Comparable<T>
Node<T>* nodeOrder;


// helper function in printTree
template<typename T>
requires Comparable<T>
void printHelper(HDC hdc, Node<T>* node, int ellipseXX, int ellipseYY, int& radius, int& radius2)
{
    if (node->key >= 10) {
        radius2 = 10;
    }
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);
    
    HFONT hf;
    long lfHeight;
    
    HFONT hFont1 = CreateFont(radius2 * 2, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
    HFONT hFontOriginal = (HFONT)SelectObject(hdc, hFont1);
    
    auto str = std::to_string(node->key);
    std::wstring stemp = std::wstring(str.begin(), str.end());
    LPCWSTR sw = stemp.c_str();
    
    RECT textRect = { ellipseXX - radius / 2, ellipseYY - radius2, ellipseXX + radius / 2, ellipseYY + radius };

    DrawText(hdc, sw, -1, &textRect, DT_SINGLELINE | DT_NOCLIP);
    radius2 = 20;
}

// print tree in the window
void printTree(HWND hWnd, HDC hdc) {

    if (guard) {
        HFONT hFont3 = CreateFont(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
        hWndEdit = CreateWindow(TEXT("EDIT"), TEXT("6"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE, 0, 5, 40, 40, hWnd, NULL, NULL, NULL);
        SendMessage(hWndEdit, WM_SETFONT, WPARAM(hFont3), TRUE);

        guard = false;
    }

    RECT rect;
    int width = 0;
    int height = 0;
    auto radius = 20;
    auto radius2 = 20;

    if (GetWindowRect(hWnd, &rect))
    {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

    std::vector<std::vector<std::pair<int, int>>> graphPoints;

    std::vector<std::vector<std::pair<int, bool>>> graphPointsHelper;

    int k = 1;
    int heightTemp = radius * 2;
    int widthTemp = width / 2;

    for (int i = 0; i < 6; ++i) {
        std::vector<std::pair<int, int>> tempVec{};
        std::vector<std::pair<int, bool>> tempVecHelper{};
        int tempWidth2 = widthTemp;
        for (int j = 1; j <= k; ++j) {

            auto pair = std::make_pair(tempWidth2, heightTemp);
            tempVec.push_back(pair);
            auto pair2 = std::make_pair(-1, false);
            tempVecHelper.push_back(pair2);
            tempWidth2 += widthTemp * 2;
        }

        graphPoints.push_back(tempVec);
        graphPointsHelper.push_back(tempVecHelper);
        k *= 2;
        heightTemp += radius * 4;
        widthTemp /= 2;
    }

    Gdiplus::Pen blackPen(Gdiplus::Color(255, 0, 0, 0), 1);

    Gdiplus::Pen greenPen(Gdiplus::Color(255, 0, 255, 0), 2);

    Gdiplus::Graphics gs(hdc);
    // Tree processing
    auto root = rbtree<int>.GetRoot();
    auto orderStat = rbtree<int>.getOrderStatistic(ORDER_STAT);

    auto orderStatString = "-th order statistic: " + std::to_string(orderStat);
    std::wstring stemp = std::wstring(orderStatString.begin(), orderStatString.end());
    LPCWSTR sw3 = stemp.c_str();

    HFONT hFont2 = CreateFont(40, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
    HFONT hFontOriginal = (HFONT)SelectObject(hdc, hFont2);

    RECT textRect = { 40 , 0, 100, 40 };

    DrawText(hdc, sw3, -1, &textRect, DT_SINGLELINE | DT_NOCLIP);

    std::vector<Node<int>*> nodesAtHeight;

    std::vector<std::vector<Node<int>*>> vectorOfNodesAtHeight;

    nodesAtHeight.push_back(root);
    bool isNullptr = false;

    for (int k = 0; k < 6; ++k) {
        vectorOfNodesAtHeight.push_back(nodesAtHeight);
        std::vector<Node<int>*> nodesAtHeightTemp;
        for (int i = 0; i < nodesAtHeight.size(); ++i) {
            if (nodesAtHeight[i] == nullptr) {
                nodesAtHeightTemp.push_back(nullptr);
                nodesAtHeightTemp.push_back(nullptr);
            }
            else {
                nodesAtHeightTemp.push_back(nodesAtHeight[i]->left);
                nodesAtHeightTemp.push_back(nodesAtHeight[i]->right);
            }
        }
        nodesAtHeight = nodesAtHeightTemp;
    }

    for (int i = 0; i < graphPoints.size(); ++i) {
        for (int j = 0; j < graphPoints[i].size(); ++j) {
            auto ellipseX = graphPoints[i][j].first;
            auto ellipseY = graphPoints[i][j].second;

            if (i < graphPoints.size() - 1) {
                if (vectorOfNodesAtHeight[i + 1][2 * j] != nullptr) {
                    gs.DrawLine(&blackPen, ellipseX, ellipseY, graphPoints[i + 1][2 * j].first, graphPoints[i + 1][2 * j].second);
                }
                if (vectorOfNodesAtHeight[i + 1][2 * j + 1] != nullptr) {
                    gs.DrawLine(&blackPen, ellipseX, ellipseY, graphPoints[i + 1][2 * j + 1].first, graphPoints[i + 1][2 * j + 1].second);
                }
            }
            if (vectorOfNodesAtHeight[i][j] != nullptr) {

                auto sizeOfTheTree = vectorOfNodesAtHeight[i][j]->key;
                if (sizeOfTheTree == orderStat) {
                    nodeOrder<int> = vectorOfNodesAtHeight[i][j];
                    SelectObject(hdc, GetStockObject(DC_BRUSH));

                    SetDCBrushColor(hdc, RGB(0, 255, 0));

                    Ellipse(hdc, ellipseX - radius - 5, ellipseY - radius - 5, ellipseX + radius + 5, ellipseY + radius + 5);

                    int k = i, l = j;
                    while (vectorOfNodesAtHeight[k][l]->parent != nullptr) {
                        auto ellipseXX = graphPoints[k][l].first;
                        auto ellipseYY = graphPoints[k][l].second;
                        gs.DrawLine(&greenPen, ellipseXX, ellipseYY, graphPoints[k-1][l/2].first, graphPoints[k-1][l/2].second);

                        if (vectorOfNodesAtHeight[k][l]->color == ::Color::Red) {
                            SelectObject(hdc, GetStockObject(DC_BRUSH));

                            SetDCBrushColor(hdc, RGB(255, 0, 0));
                        }
                        else {
                            SelectObject(hdc, GetStockObject(DC_BRUSH));

                            SetDCBrushColor(hdc, RGB(0, 0, 0));
                        }

                        Ellipse(hdc, ellipseXX - radius, ellipseYY - radius, ellipseXX + radius, ellipseYY + radius);

                        printHelper(hdc, vectorOfNodesAtHeight[k][l], ellipseXX, ellipseYY, radius, radius2);
                        
                        k = k - 1;
                        l = l / 2;
                    }
                    if (vectorOfNodesAtHeight[0][0]->color == ::Color::Red) {
                        SelectObject(hdc, GetStockObject(DC_BRUSH));

                        SetDCBrushColor(hdc, RGB(255, 0, 0));
                    }
                    else {
                        SelectObject(hdc, GetStockObject(DC_BRUSH));

                        SetDCBrushColor(hdc, RGB(0, 0, 0));
                    }
                    
                    Ellipse(hdc, graphPoints[0][0].first - radius, graphPoints[0][0].second - radius, graphPoints[0][0].first + radius, graphPoints[0][0].second + radius);

                    printHelper(hdc, vectorOfNodesAtHeight[0][0], graphPoints[0][0].first, graphPoints[0][0].second, radius, radius2);
                }

                if (vectorOfNodesAtHeight[i][j]->color == ::Color::Red) {
                    SelectObject(hdc, GetStockObject(DC_BRUSH));

                    SetDCBrushColor(hdc, RGB(255, 0, 0));
                }
                else {
                    SelectObject(hdc, GetStockObject(DC_BRUSH));

                    SetDCBrushColor(hdc, RGB(0, 0, 0));
                }

                Ellipse(hdc, ellipseX - radius, ellipseY - radius, ellipseX + radius, ellipseY + radius);

                if (sizeOfTheTree >= 10) {
                    radius2 = 10;
                }
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkMode(hdc, TRANSPARENT);

                HFONT hf;
                long lfHeight;

                HFONT hFont1 = CreateFont(radius2 * 2, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                    CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
                HFONT hFontOriginal = (HFONT)SelectObject(hdc, hFont1);

                auto str = std::to_string(sizeOfTheTree);
                std::wstring stemp = std::wstring(str.begin(), str.end());
                LPCWSTR sw = stemp.c_str();

                RECT textRect = { ellipseX - radius / 2, ellipseY - radius2, ellipseX + radius / 2, ellipseY + radius };

                DrawText(hdc, sw, -1, &textRect, DT_SINGLELINE | DT_NOCLIP);
                radius2 = 20;

            }
        }
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    //case WM_CREATE:
        // create a 1 second timer 
        
        //return 0;
    case WM_COMMAND:
        {
        switch (HIWORD(wParam))
        {
        case EN_CHANGE:

            auto length = GetWindowTextLength(hWndEdit) + 1;
            std::vector<wchar_t> list(length);
            GetWindowText(hWndEdit, &list[0], length);

            std::string tempString="";
            for (int i = 0; i < list.size(); ++i) {
                tempString += list[i];

            }
            try {
                auto intValueOfOrder = std::stoi(tempString);
                if (intValueOfOrder < 1) {
                    break;
                }
                ORDER_STAT = intValueOfOrder;

                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
            }
            catch (std::exception& ex) {
                // do nothing here
            }
            break;
        }
       

            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            
            HDC hdc = BeginPaint(hWnd, &ps);
            
            
            // TODO: Add any drawing code that uses hdc here...
            
            // printing the tree
            printTree(hWnd, hdc);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:

        //AddMenuStrip(hWnd);
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
