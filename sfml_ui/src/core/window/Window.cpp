#include <Window.hpp>
#include <print>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <Page.hpp>

namespace sfui {

    Window::Window(const int &width, const int &heigth, const WindowState &winsowState)
        :m_winsowState(winsowState), m_event() {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        m_screenSize.x = desktopMode.width;
        m_screenSize.y = desktopMode.height;
        m_windowSize = WindowSize(width, heigth);
        if (winsowState == WindowState::Windowed) {
            m_sf_renderWindow.create(sf::VideoMode(width, heigth), "");
        } else {
            m_sf_renderWindow.create(sf::VideoMode::getDesktopMode(),
                "", sf::Style::Fullscreen);
        }
    }

    bool Window::init(const int &framerateLimit, const bool &verticalSyncEnabled) {
        m_framerateLimit = framerateLimit;
        m_verticalSyncEnabled = verticalSyncEnabled;
        m_sf_renderWindow.setVerticalSyncEnabled(m_verticalSyncEnabled);
        m_sf_renderWindow.setFramerateLimit(m_framerateLimit);
        return false;
    }

    void Window::addPage(const Title &pageTitle, PagePtr<Page> page) {
        m_pages[pageTitle] = move(page);
        m_pages[pageTitle]->setWindow(this);
        m_pages[pageTitle]->setMouseWindow(&m_sf_renderWindow);
    }

    void Window::startShow(const Title &firstPageTitle) {
        //�л����׽���
        requestPageSwitch(firstPageTitle);
        m_sf_renderWindow.setTitle(m_nowPageTitle);
        //�������¸ô��ڣ�ֱ������ر�
        while (m_sf_renderWindow.isOpen()) {

            // ��ȡ������Ϣ
            procesMessage();

            // ���½�������
            m_pages[m_nowPageTitle]->update();

            // ��ʾһ֡�Ļ���
            drawFrame();
        }
    }
    //�������ںʹ��ڵ�ǰ�������Ϣ
    void Window::procesMessage() {
        // ��������ʵʱ��Ϣ
        handleRealTimeInput();
        // ����ҳ��ʵʱ��Ϣ
        m_pages[m_nowPageTitle]->executeKeyPressOnce();
        while (m_sf_renderWindow.pollEvent(m_event)) {
            // ���������¼���Ϣ
            handleEventInput();
            // ����ҳ���¼���Ϣ
            m_pages[m_nowPageTitle]->executeEventBinding(m_event);
        }
    }
    void Window::handleEventInput() {
        if (m_event.type == sf::Event::Closed) {
            m_sf_renderWindow.close();
        } else if (m_event.type == sf::Event::KeyPressed) {
            if (m_event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
            }
        }
    }
    void Window::handleRealTimeInput() {

    }

    void Window::drawFrame() {
        updateView();
        // ����ҳ�棬����ҳ���ͼ�μ��ص�����
        m_pages[m_nowPageTitle]->updateFrame();
        // ��ʾ��ǰ���ڵĻ���
        m_sf_renderWindow.display();
        m_sf_renderWindow.clear(m_pages[m_nowPageTitle]->getBackgroundColor());

    }

    void Window::requestPageSwitch(const Title &pageTitle) {
        // ���Ҫ�л�ҳ���Ƿ���ڣ��������ڣ��׳�һ���쳣
        try {
            if (m_pages.find(pageTitle) == m_pages.end()) {
                throw std::runtime_error("Page don't exist!!!");
            }
        } catch (const std::runtime_error &err) {
            std::wcerr << "Exception: " << pageTitle << err.what() << std::endl;
        }

        m_nowPageTitle = pageTitle;
        m_sf_renderWindow.setTitle(m_nowPageTitle);
        // ��ʼ������
        m_pages[m_nowPageTitle]->init();
    }


    sf::RenderWindow &Window::getSfRenderWindow() {
        return m_sf_renderWindow;
    }

    const WindowSize Window::getWindowSize() {
        //return m_windowSize;
        return m_sf_renderWindow.getSize();
    }
    const WindowSize &Window::getScreenSize() const {
        return m_screenSize;
    }
    void Window::updateView() {
        m_pages[m_nowPageTitle]->updateView();
        //m_sf_renderWindow.setView(m_pages[m_nowPageTitle]->getView());
    }

    // �л�����ģʽ
    void Window::toggleFullscreen() {
        if (m_winsowState == WindowState::Windowed) {
            toFullscreen();
        } else {
            toWindowed();
        }
        // �ڴ����л�ʱ���Ὣ���ڱ����������ʹȫ��ģʽ�����������ڱ��⣬���Ƿ�����������
        // �����Կ��Կ������ڱ��⣬������л����ں�Ӧ�����ڱ�����������
        m_sf_renderWindow.setTitle(m_nowPageTitle);
    }

    // �л�Ϊȫ��ģʽ
    void Window::toFullscreen() {
        m_winsowState = WindowState::Fullscreen;
        m_windowSize = m_sf_renderWindow.getSize();
        m_sf_renderWindow.create(sf::VideoMode::getDesktopMode(),
            "", sf::Style::Fullscreen);
        m_sf_renderWindow.setVerticalSyncEnabled(m_verticalSyncEnabled);
        m_sf_renderWindow.setFramerateLimit(m_framerateLimit);
    }

    //�л�Ϊ����ģʽ
    void Window::toWindowed() {
        m_winsowState = WindowState::Windowed;
        m_sf_renderWindow.create(sf::VideoMode(m_windowSize.x, m_windowSize.y), "");
        m_sf_renderWindow.setVerticalSyncEnabled(m_verticalSyncEnabled);
        m_sf_renderWindow.setFramerateLimit(m_framerateLimit);
    }
}