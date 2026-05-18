#include "UiState/UiState.hpp"
#include "InformationUnit/InformationUnit.hpp"
#include "Networker/Networker.hpp"


LoadingPage::LoadingPage(Queue &queue): eventQueue(queue) {
    renderer = ftxui::Renderer([this]() {
       return ftxui::paragraphAlignCenter("Loading...");
    });
}

ftxui::Component LoadingPage::getRenderer() {
    return renderer;
}



