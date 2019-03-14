
int2
Win32CalcWindowPosition(i32 WindowWidth, i32 WindowHeight) {
    i32 ScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
    i32 ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    i32 X = (ScreenWidth / 2) - (WindowWidth / 2);
    i32 Y = (ScreenHeight / 2) - (WindowHeight / 2);
    return int2(X, Y);
}
