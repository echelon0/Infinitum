
char *
CreateErrorMessage(char *SimpleMsg, HRESULT hr) {
    _com_error err(hr);
    char *ErrorMsg = (char *)malloc((StrLen(SimpleMsg) + StrLen((char *)err.ErrorMessage())) * sizeof(char) + (sizeof(char) * 3));
    StrCopy(ErrorMsg, SimpleMsg);
    StrCat(ErrorMsg, "\n\n");
    StrCat(ErrorMsg, (char *)err.ErrorMessage());
    return ErrorMsg;
}

void
DisplayErrorMessageBox(char *Title, char *SimpleMsg, HRESULT hr) {
    char *ErrorMsg = CreateErrorMessage(SimpleMsg, hr); 
    MessageBoxA(0, ErrorMsg, Title, MB_OK|MB_ICONERROR);
    free(ErrorMsg);
}

#define ReturnOnError(hr, Title, Message)               \
    if(FAILED(hr)) {                                    \
        DisplayErrorMessageBox(Title, Message, hr);     \
        return false;                                   \
    }                                                   \
