#include "MenuPage.hpp"

MenuPage::MenuPage() {
    button = ftxui::Button("siema", []() {
        return;
    });

    container = ftxui::Container::Vertical({button});
}


ftxui::Component MenuPage::getContainer() {
    return container;
}
