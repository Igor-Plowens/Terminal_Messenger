#include "UiState.hpp"

UiState::UiState(ftxui::ScreenInteractive &screen, Queue &queue):
        screen(screen),

        loadingPage(queue),
        startingPage(queue),
        loginPage(queue),
        registerPage(queue),
        menuPage(queue),
        dmPage(queue)
{
        container = ftxui::Container::Tab({
            loadingPage.getRenderer(),
            startingPage.getContainer(),
            loginPage.getRenderer(),
            registerPage.getRenderer(),
            menuPage.getRenderer(),
            dmPage.getRenderer()
        }, &selector);
}

ftxui::Component UiState::getFinalContainer() {
    return container;
}
