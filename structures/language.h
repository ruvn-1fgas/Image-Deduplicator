#include <map>
#include <string>

namespace language
{
    /*
        <key> = {
            <language> => <value>
            language is int
        }
    */

    std::map<std::string, std::map<int, std::string>> dict = {
    {"OpenDirButtonLabel", {
        {0, "Open directory"},
        {1, "Выбрать директорию"},
    }},
    {"SelectedDirectoryLabel", {
        {0, "Selected directory - "},
        {1, "Текущая директория - "},
    }},
    {"ExcludeDirButtonLabel", {
        {0, "Exclude directory"},
        {1, "Исключить директорию"},
    }},
    {"ExcludeDirLabel", {
        {0, "Excluded directories - "},
        {1, "Исключенные директории - "},
    }},
    {"StartButtonLabel", {
        {0, "Start"},
        {1, "Начать"},
    }},
    {"StartButton.Error.DirectorNotChosen", {
        {0, "Select Directory"},
        {1, "Выберите директорию"},
    }},
    {"StartButton.Error.DirectoryNotExists", {
        {0, "Directory not exists"},
        {1, "Выбранная директория не существует"},
    }},
    {"StartButton.Result.NoDuplicates", {
        {0, "Duplicates not found"},
        {1, "Дубликатов не найдено"},
    }},
    {"StartButton.HashCalcAction", {
        {0, "Hash calculation - "},
        {1, "Вычисление хеша - "},
    }},
    {"StartButton.ImageCompareAction", {
        {0, "Image comparison - "},
        {1, "Сравнение изображений - "},
    }},
    {"OpenDialog.TitleBar", {
        {0, "Select Directory"},
        {1, "Выберите директорию"},
    }},
    {"Dialog.CancelOption", {
        {0, "Cancel"},
        {1, "Отмена" },
    }},
    {"Dialog.AcceptOption", {
        {0, "Accept"},
        {1, "Выбрать"},
    }},
    {"Dialog.ApplyOption", {
        {0, "Apply"},
        {1, "Применить"},
    }},
    {"Dialog.SelectOption", {
        {0, "Select Directory"},
        {1, "Выбрать"},
    }},
    {"Dialog.OpenOption", {
        {0, "Select Directory"},
        {1, "Выбрать"},
    }},
    {"OpenDialog.Selected.Text", {
        {0, "Selected directory - "},
        {1, "Текущая директория - "},
    }},
    {"ExcludeDialog.TitleBar", {
        {0, "Select Directory"},
        {1, "Выберите директорию"},
    }},
    {"ExcludeDialog.Error.DirectoryNotChosen", {
        {0, "First select directory"},
        {1, "Сначала выберите директорию"},
    }},
    {"ExcludeDialog.Error.DirectoryIsSubDir", {
        {0, "Selected directory is subdirectory of excluded directory"},
        {1, "Выбранная директория является поддиректорией исключенной директории"},
    }},
    {"ExcludeDialog.Error.DirectoryIsAlreadyExcluded", {
        {0, "Directory already excluded"},
        {1, "Выбранная директория уже добавлена в список исключений"},
    }},
    {"ExcludeDialog.Error.DirectoryIsNotSubDir", {
        {0, "Selected directory is not subdirectory of current directory"},
        {1, "Выбранная директория не является поддиректорией текущей директории"},
    }},
    {"ExcludedDirectories.Label", {
        {0, "Excluded directories - "},
        {1, "Исключенные директории - "},
    }},
    {"SettingsDialog.TitleBar", {
        {0, "Settings"},
        {1, "Настройки"},
    }},
    {"SettingsDialog.RecursiveLabel", {
        {0, "Recursive search"},
        {1, "Рекурсивный поиск"},
    }},
    {"SettingsDialog.HashThresholdLabel", {
        {0, "Hash threshold"},
        {1, "Порог схожести"},
    }},
    {"SettingsDialog.ThreadCount.Text", {
        {0, "Tread count"},
        {1, "Количество потоков" },
    }},
    {"SettingsDialog.ThemeLabel", {
        {0, "App theme"},
        {1, "Тема приложения"},
    }},
    {"SettingsDialog.ThemeLight", {
        {0, "Light"},
        {1, "Светлая"},
    }},
    {"SettingsDialog.ThemeDark", {
        {0, "Dark"},
        {1, "Темная"},
    }},
    {"SettingsDialog.Language", {
        {0, "Language"},
        {1, "Язык приложения"},
    }},
    {"duplWindow.TitleBar", {
        {0, "Duplicates"},
        {1, "Дубликаты"},
    }},
    {"duplWindow.PrevButton", {
        {0, "Previous"},
        {1, "Назад"},
    }},
    {"duplWindow.NextButton", {
        {0, "Next"},
        {1, "Вперед"},
    }},
    {"duplWindow.ListBox.Label", {
        {0, "List "},
        {1, "Список "},
    }},
    {"duplWindow.ListBox.RightClick.Open", {
        {0, "Open"},
        {1, "Открыть"},
    }},
    {"duplWindow.ListBox.RightClick.Delete", {
        {0, "Delete"},
        {1, "Удалить"},
    }},
    {"duplWindow.ListBox.OpenFile.Error.FileNotFound", {
        {0, "Image not found"},
        {1, "Изображение не найдено"},
    }},
};
    
    // getting value from dict
    // language::dict["OpenDirButton"][0] - returns "Open directory"
};