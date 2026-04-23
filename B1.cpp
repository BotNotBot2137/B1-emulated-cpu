#include <SMLF/Graphics.hpp>

int main(){
    SMLF::Window window("SMLF", 255, 255);
    SMLF::Circle circle(100, 100, 100);
    circle.setFillColor(SMLF::Color::Red);
    while (window.isOpen()) {
    std::cout << "Window open";
    return 0;
}