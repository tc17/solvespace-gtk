#include <string>
#include "solvespace.h"

std::string RecentFile[MAX_RECENT];

void CheckMenuById(int id, bool checked)
{
}

void EnableMenuById(int id, bool checked)
{
}

void DoMessageBox(char *str, int rows, int cols, bool error)
{
}

void InvalidateText(void)
{
}

void InvalidateGraphics(void)
{
}

void AddContextMenuItem(const char *label, int id)
{
}

int ShowContextMenu(void)
{
}

void CreateContextSubmenu(void)
{
}

void PaintGraphics(void)
{
}

void HideTextEditControl(void)
{
}

void ShowTextEditControl(int x, int y, char *s)
{
}

bool TextEditControlIsVisible(void)
{
}	

void HideGraphicsEditControl(void)
{
}

void ShowGraphicsEditControl(int x, int y, char *s)
{
}

bool GraphicsEditControlIsVisible(void)
{
}

void GetTextWindowSize(int *w, int *h)
{
}

void ShowTextWindow(bool visible)
{
}

void GetGraphicsWindowSize(int *w, int *h)
{
}

bool GetSaveFile(std::string *file, const char *defExtension, const char *selPattern)
{
}

bool GetOpenFile(std::string *file, const char *defExtension, const char *selPattern)
{
}

void SetMousePointerToHand(bool yes)
{
}

void MoveTextScrollbarTo(int pos, int maxPos, int page)
{
}

void SetTimerFor(int milliseconds)
{
}

void OpenWebsite(const char *url)
{
}

void SetWindowTitle(const char *str)
{
}

void RefreshRecentMenus(void)
{
}

saveRv SaveFileYesNoCancel(void)
{
}

void LoadAllFontFiles(void)
{
}

void ExitNow(void)
{
}
