#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <cstdlib>
#include <ctime>

// список рёбер
std::vector<std::pair<int, int>> edges;

// матрица смежности (двумерный динамический массив)
int** smezhMatrix = nullptr;

// список смежности (массив векторов)
const int numVertices = 5;
std::vector<int> smezhList[numVertices];

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
// радиус большой окружности
const float RADIUS = 200;
// радиус вершины
const float CIRCLE_RADIUS = 30;

// цвета для рёбер
std::vector<sf::Color> edgeColors;

// Функция для генерации уникальных случайных цветов
void generateUniqueColors(size_t count) {
    std::set<std::tuple<int, int, int>> uniqueColors;
    while (uniqueColors.size() < count) {
        auto color = std::make_tuple(rand() % 256, rand() % 256, rand() % 256);
        uniqueColors.insert(color);
    }
    for (auto color : uniqueColors) {
        edgeColors.emplace_back(std::get<0>(color), std::get<1>(color), std::get<2>(color));
    }
}

// Добавление рёбер в граф
void addEdge(int from, int to) {
    edges.push_back({from, to});

    // матрица смежности:
    smezhMatrix[from - 1][to - 1] = 1;

    // список смежности
    smezhList[from - 1].push_back(to);
}

// Список рёбер
void printEdgeList() {
    std::cout << "Список рёбер:" << std::endl;
    for (const auto& edge : edges) {
        std::cout << "{" << edge.first << "," << edge.second << "} ";
    }
    std::cout << std::endl;
}

// Матрица смежности
void printMatrix() {
    std::cout << "Матрица смежности:" << std::endl;
    std::cout << "   ";
    for (int i = 1; i <= numVertices; ++i) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "   ";
    for (int i = 0; i < numVertices; ++i) {
        std::cout << "_ ";
    }
    std::cout << std::endl;

    for (int i = 0; i < numVertices; ++i) {
        std::cout << i + 1 << "| ";
        for (int j = 0; j < numVertices; ++j) {
            std::cout << smezhMatrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Список смежности
void printList() {
    std::cout << "Список смежности:" << std::endl;
    for (int i = 0; i < numVertices; ++i) {
        std::cout << i + 1 << "| ";
        for (int neigb : smezhList[i]) {
            std::cout << neigb << " ";
        }
        std::cout << std::endl;
    }
}

// Координаты вершины
std::pair<float, float> getVertexPosition(int index, int totalVert) {
    float angle = 2 * M_PI * index / totalVert;
    float x = WINDOW_WIDTH / 2 + RADIUS * cos(angle);
    float y = WINDOW_HEIGHT / 2 + RADIUS * sin(angle);
    return {x, y};
}

// Рисование стрелки
void drawTriangle(sf::RenderWindow& window, std::pair<float, float> start, std::pair<float, float> end, sf::Color color, float offsetAngle = 0) {
    float angle = atan2(end.second - start.second, end.first - start.first) + offsetAngle;

    float baseWidth = 2 * CIRCLE_RADIUS;

    sf::Vector2f tip(end.first, end.second);

    sf::Vector2f baseLeft(
            start.first + (baseWidth / 2) * cos(angle + M_PI / 2),
            start.second + (baseWidth / 2) * sin(angle + M_PI / 2)
    );

    sf::Vector2f baseRight(
            start.first + (baseWidth / 2) * cos(angle - M_PI / 2),
            start.second + (baseWidth / 2) * sin(angle - M_PI / 2)
    );

    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, tip);
    triangle.setPoint(1, baseLeft);
    triangle.setPoint(2, baseRight);
    triangle.setFillColor(color);

    window.draw(triangle);
}

// Рисование графа
void drawGraph(sf::RenderWindow& window, const std::vector<std::pair<int, int>>& edges, sf::Font& font) {
    int vertexCount = numVertices;

    std::set<std::pair<int, int>> edgeSet(edges.begin(), edges.end());

    for (size_t i = 0; i < edges.size(); ++i) {
        int from = edges[i].first;
        int to = edges[i].second;

        std::pair<float, float> start = getVertexPosition(from - 1, vertexCount);
        std::pair<float, float> end = getVertexPosition(to - 1, vertexCount);

        bool isBidirectional = edgeSet.count({to, from}) > 0;

        float offsetAngle = 0.0f;
        if (isBidirectional && from < to) {
            offsetAngle = 0.05f;
        } else if (isBidirectional && from > to) {
            offsetAngle = -0.05f;
        }

        drawTriangle(window, start, end, edgeColors[i % edgeColors.size()], offsetAngle);
    }

    for (int i = 0; i < vertexCount; ++i) {
        std::pair<float, float> position = getVertexPosition(i, vertexCount);

        sf::CircleShape vertex(CIRCLE_RADIUS);
        vertex.setFillColor(sf::Color::White);
        vertex.setOutlineColor(sf::Color::Black);
        vertex.setOutlineThickness(4);
        vertex.setOrigin(CIRCLE_RADIUS, CIRCLE_RADIUS);
        vertex.setPosition(position.first, position.second);

        window.draw(vertex);

        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(i + 1));
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
        text.setPosition(position.first, position.second);

        window.draw(text);
    }
}

int main() {
    // Выделение памяти под матрицу смежности
    smezhMatrix = new int*[numVertices];
    for (int i = 0; i < numVertices; ++i) {
        smezhMatrix[i] = new int[numVertices];
    }

    // Инициализация матрицы нулями
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            smezhMatrix[i][j] = 0;
        }
    }

    srand(static_cast<unsigned int>(time(nullptr))); // Инициализация случайного генератора

    addEdge(1, 2);
    addEdge(1, 3);
    addEdge(1, 5);
    addEdge(2, 1);
    addEdge(3, 1);
    addEdge(3, 2);
    addEdge(4, 5);
    addEdge(4, 3);
    addEdge(5, 3);
    addEdge(5, 4);

    printEdgeList();
    printMatrix();
    printList();

    // Генерация уникальных случайных цветов для рёбер
    generateUniqueColors(edges.size());

    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        std::cout << "Ошибка загрузки шрифта!" << std::endl;
        return -1;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Graph Visualization");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        drawGraph(window, edges, font);

        window.display();
    }

    // Очистка памяти, выделенной под матрицу смежности
    for (int i = 0; i < numVertices; ++i) {
        delete[] smezhMatrix[i];
    }
    delete[] smezhMatrix;

    return 0;
}
