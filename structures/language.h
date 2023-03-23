#include <map>
#include <string>

namespace language
{
    /*
    <language> = {
        <key> = <value>
    }
    */

    std::map<std::string, std::map<std::string, std::string>> dict = {
        {"en", {{"mainWindow.OpenDirButton", "Open directory"}, {"mainWindow.DirLabel", "Selected directory - "}, {"mainWindow.ExcludeDirButton", "Exclude directory"}, {"mainWindow.ExcludeDirLabel", "Excluded directories - "}, {"mainWindow.StartButton", "Start"}, {"mainWindow.StartButton.Error.NotChosenDir", "Select Directory"}, {"mainWindow.StartButton.Error.DirectoryNotExists", "Directory not exists"}, {"mainWindow.StartButton.Result.NoDuplicates", "Duplicates not found"}, {"mainWindow.StartButton.HashCalc", "Hash calculation - "}, {"mainWindow.StartButton.ImageCompare", "Image comparison - "}, {"mainWindow.OpenDialog.TitleBar", "Select Directory"}, {"mainWindow.OpenDialog.Cancel", "_Cancel"}, {"mainWindow.OpenDialog.Accept", "_Accept"}, {"mainWindow.OpenDialog.Selected.Text", "Selected directory - "}, {"mainWindow.ExcludeDialog.TitleBar", "Select Directory"}, {"mainWindow.ExcludeDialog.Cancel", "_Cancel"}, {"mainWindow.ExcludeDialog.Accept", "_Select Directory"}, {"mainWindow.ExcludeDialog.Error.NotChosenDir", "First select directory"}, {"mainWindow.ExcludeDialog.Error.DirectoryIsSubDir", "Selected directory is subdirectory of excluded directory"}, {"mainWindow.ExcludeDialog.Error.DirectoryAlreadyExcluded", "Directory already excluded"}, {"mainWindow.ExcludeDialog.Error.DirectoryIsNotSubDir", "Selected directory is not subdirectory of current directory"}, {"mainWindow.ExcludeDialog.Selected.Text", "Excluded directories - "}, {"mainWindow.SettingsDialog.TitleBar", "Settings"}, {"mainWindow.SettingsDialog.Cancel", "Cancel"}, {"mainWindow.SettingsDialog.Apply", "Apply"}, {"mainWindow.SettingsDialog.RecursiveLabel", "Recursive search"}, {"mainWindow.SettingsDialog.HashThresholdLabel", "Hash threshold"}, {"mainWindow.SettingsDialog.ThemeLabel", "App theme"}, {"mainWindow.SettingsDialog.ThemeLight", "Light"}, {"mainWindow.SettingsDialog.ThemeDark", "Dark"}, {"mainWindow.SettingsDialog.Language", "Language"}, {"duplWindow.Title", "Duplicates"}, {"duplWindow.PrevButton", "Previous"}, {"duplWindow.NextButton", "Next"}, {"duplWindow.ListBox.Label", "List "}, {"duplWindow.ListBox.RightClick.Open", "Open"}, {"duplWindow.ListBox.RightClick.Delete", "Delete"}, {"duplWindow.ListBox.OpenFile.Error.FileNotFound", "Image not found"}}},
        {"ru", {
                   {"mainWindow.OpenDirButton", "Выбрать директорию"},
                   {"mainWindow.DirLabel", "Текущая директория - "},
                   {"mainWindow.ExcludeDirButton", "Исключить директорию"},
                   {"mainWindow.ExcludeDirLabel", "Исключенные директории - "},
                   {"mainWindow.StartButton", "Начать"},
                   {"mainWindow.StartButton.Error.NotChosenDir", "Выберите директорию"},
                   {"mainWindow.StartButton.Error.DirectoryNotExists", "Директория не существует"},
                   {"mainWindow.StartButton.Result.NoDuplicates", "Дубликатов не найдено"},
                   {"mainWindow.StartButton.HashCalc", "Вычисление хеша - "},
                   {"mainWindow.StartButton.ImageCompare", "Сравнение изображений - "},
                   {"mainWindow.OpenDialog.TitleBar", "Выберите директорию"},
                   {"mainWindow.OpenDialog.Cancel", "_Отмена"},
                   {"mainWindow.OpenDialog.Accept", "_Выбрать"},
                   {"mainWindow.OpenDialog.Selected.Text", "Текущая директория - "},
                   {"mainWindow.ExcludeDialog.TitleBar", "Выберите директорию"},
                   {"mainWindow.ExcludeDialog.Cancel", "_Отмена"},
                   {"mainWindow.ExcludeDialog.Accept", "_Выбрать"},
                   {"mainWindow.ExcludeDialog.Error.NotChosenDir", "Сначала выберите директорию"},
                   {"mainWindow.ExcludeDialog.Error.DirectoryIsSubDir", "Выбранная директория является поддиректорией исключенной директории"},
                   {"mainWindow.ExcludeDialog.Error.DirectoryAlreadyExcluded", "Директория уже добавлена в список исключений "},
                   {"mainWindow.ExcludeDialog.Error.DirectoryIsNotSubDir", "Выбранная директория не является поддиректорией текущей директории"},
                   {"mainWindow.ExcludeDialog.Selected.Text", "Исключенные директории - "},
                   {"mainWindow.SettingsDialog.TitleBar", "Настройки"},
                   {"mainWindow.SettingsDialog.Cancel", "Отмена"},
                   {"mainWindow.SettingsDialog.Apply", "Применить"},
                   {"mainWindow.SettingsDialog.RecursiveLabel", "Рекурсивный поиск"},
                   {"mainWindow.SettingsDialog.HashThresholdLabel", "Порог схожести"},
                   {"mainWindow.SettingsDialog.ThemeLabel", "Тема приложения"},
                   {"mainWindow.SettingsDialog.ThemeLight", "Светлая"},
                   {"mainWindow.SettingsDialog.ThemeDark", "Темная"},
                   {"mainWindow.SettingsDialog.Language", "Язык приложения"},
                   {"duplWindow.Title", "Дубликаты"},
                   {"duplWindow.PrevButton", "Назад"},
                   {"duplWindow.NextButton", "Вперед"},
                   {"duplWindow.ListBox.Label", "Список "},
                   {"duplWindow.ListBox.RightClick.Open", "Открыть"},
                   {"duplWindow.ListBox.RightClick.Delete", "Удалить"},
                   {"duplWindow.ListBox.OpenFile.Error.FileNotFound", "Изображение не найдено"},
               }}}; // languages
    // dict["en"]["mainWindow.OpenDirButton"] = "Open directory";
};