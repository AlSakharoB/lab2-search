/**
 * @file search_algorithms.cpp
 * @brief Реализация различных алгоритмов поиска для объектов Passenger.
 * 
 * Включает линейный поиск, бинарное дерево поиска (BST), красно-чёрное дерево (RBTree),
 * хеш-таблицу с цепочками и сравнение с std::multimap.
 * Измеряется время поиска и количество коллизий для хеш-таблицы.
 * 
 * @author <Alsakh>
 * @date 2025
 */
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <fstream>
#include <map>

/**
 * @struct Passenger
 * @brief Структура описывает пассажира с основными полями.
 * 
 * Ключом поиска служит поле fullName (ФИО).
 */
struct Passenger {
    std::string fullName;       /**< Полное имя пассажира (ключ поиска) */
    int         cabinNumber;    /**< Номер каюты */
    std::string cabinType;      /**< Тип каюты (Люкс, 1, 2, 3) */
    std::string destinationPort;/**< Порт назначения */
};

/**
 * @brief Выполняет линейный поиск всех вхождений ключа в массив пассажиров.
 * 
 * @param arr Вектор пассажиров.
 * @param key Ключ поиска — полное имя.
 * @return std::vector<size_t> Вектор индексов всех найденных совпадений.
 */
std::vector<size_t> linearSearch(const std::vector<Passenger>& arr,
                                 const std::string& key)
{
    std::vector<size_t> idx;
    for (size_t i = 0; i < arr.size(); ++i)
        if (arr[i].fullName == key)
            idx.push_back(i);
    return idx;
}

/**
 * @struct BSTNode
 * @brief Узел бинарного дерева поиска.
 * 
 * Хранит ключ, список указателей на пассажиров с этим ключом,
 * а также указатели на левое и правое поддерево.
 */
struct BSTNode {
    std::string               key;        /**< Ключ узла (fullName) */
    std::vector<const Passenger*> payload; /**< Все пассажиры с этим ключом */
    BSTNode*  left  = nullptr;             /**< Левое поддерево */
    BSTNode*  right = nullptr;             /**< Правое поддерево */
    ~BSTNode()      { delete left; delete right; }/**< Деструктор, освобождающий поддеревья */
};

/**
 * @class BST
 * @brief Несбалансированное бинарное дерево поиска для хранения Passenger.
 */
class BST {
    BSTNode* root = nullptr;
public:
    ~BST() { delete root; }

    void insert(const Passenger& p) {
        if (!root) {
            root = new BSTNode{p.fullName};
            root->payload.push_back(&p);
            return;
        }
        BSTNode* cur = root;
        while (true) {
            if (p.fullName == cur->key) {
                cur->payload.push_back(&p);
                return;
            } else if (p.fullName < cur->key) {
                if (!cur->left) cur->left = new BSTNode{p.fullName};
                cur = cur->left;
            } else {
                if (!cur->right) cur->right = new BSTNode{p.fullName};
                cur = cur->right;
            }
        }
    }

    std::vector<const Passenger*> search(const std::string& key) const {
        BSTNode* cur = root;
        while (cur) {
            if (key == cur->key) return cur->payload;
            cur = key < cur->key ? cur->left : cur->right;
        }
        return {};
    }
};


/**
 * @enum Color
 * @brief Цвет узла красно-чёрного дерева.
 */
enum Color { RED, BLACK };

/**
 * @struct RBTNode
 * @brief Узел красно-чёрного дерева.
 * 
 * Содержит ключ, список указателей на объекты,
 * цвет узла и связи на родителя и потомков.
 */
struct RBTNode {
    std::string               key;
    std::vector<const Passenger*> payload;
    Color    color   = RED;
    RBTNode* parent  = nullptr;
    RBTNode* left    = nullptr;
    RBTNode* right   = nullptr;
};

/**
 * @class RBTree
 * @brief Самобалансирующееся красно-чёрное дерево для поиска пассажиров.
 */
class RBTree {
    RBTNode* root = nullptr;
    //----------------------------------------
    //  Повороты
    void rotateLeft(RBTNode* x) {
        RBTNode* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else                           x->parent->right = y;
        y->left = x;
        x->parent = y;
    }
    void rotateRight(RBTNode* y) {
        RBTNode* x = y->left;
        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;
        if (!y->parent) root = x;
        else if (y == y->parent->left) y->parent->left = x;
        else                           y->parent->right = x;
        x->right = y;
        y->parent = x;
    }
    //----------------------------------------
    //  Балансировка после вставки
    void fixInsert(RBTNode* z) {
        while (z->parent && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                RBTNode* y = z->parent->parent->right; // дядя
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotateLeft(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                }
            } else {
                RBTNode* y = z->parent->parent->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotateRight(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }
public:
    ~RBTree() { destroy(root); }
    void destroy(RBTNode* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    void insert(const Passenger& p) {
        // обычная BST вставка
        RBTNode* y = nullptr;
        RBTNode* x = root;
        while (x) {
            y = x;
            if (p.fullName == x->key) {
                x->payload.push_back(&p);
                return; // ключ уже есть
            }
            x = (p.fullName < x->key) ? x->left : x->right;
        }
        RBTNode* z = new RBTNode;
        z->key = p.fullName;
        z->payload.push_back(&p);
        z->parent = y;
        if (!y)               root = z;
        else if (p.fullName < y->key) y->left  = z;
        else                           y->right = z;
        z->color = RED;
        fixInsert(z);
    }

    std::vector<const Passenger*> search(const std::string& key) const {
        RBTNode* cur = root;
        while (cur) {
            if (key == cur->key) return cur->payload;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        return {};
    }
};

/**
 * @class HashTable
 * @brief Хеш-таблица с цепочками для хранения пассажиров по ключу.
 */
class HashTable {
    struct Bucket {
        std::string            key;
        std::vector<const Passenger*> payload;
        Bucket* next = nullptr;
    };

    std::vector<Bucket*> table;
    size_t               collisions = 0;

    static size_t hashStr(const std::string& s, size_t mod) {
        // Полиномиальный rolling-hash
        const size_t P = 31;
        size_t h = 0;
        for (char c : s) h = (h * P + static_cast<size_t>(c)) % mod;
        return h;
    }
public:
    explicit HashTable(size_t nBuckets) : table(nBuckets, nullptr) {}
    ~HashTable() {
        for (auto* b : table) {
            while (b) {
                Bucket* nxt = b->next;
                delete b;
                b = nxt;
            }
        }
    }

    void insert(const Passenger& p) {
        size_t idx = hashStr(p.fullName, table.size());
        Bucket* cur = table[idx];
        if (!cur) {
            table[idx] = new Bucket{p.fullName};
            table[idx]->payload.push_back(&p);
            return;
        }
        // цепочка существует => коллизия
        collisions++;
        Bucket* prev = nullptr;
        while (cur) {
            if (cur->key == p.fullName) {
                cur->payload.push_back(&p);
                return;
            }
            prev = cur;
            cur = cur->next;
        }
        prev->next = new Bucket{p.fullName};
        prev->next->payload.push_back(&p);
    }

    std::vector<const Passenger*> search(const std::string& key) const {
        size_t idx = hashStr(key, table.size());
        Bucket* cur = table[idx];
        while (cur) {
            if (cur->key == key) return cur->payload;
            cur = cur->next;
        }
        return {};
    }

    size_t collisionCount() const { return collisions; }
};

/**
 * @brief Генерирует случайную строку из строчных букв латинского алфавита.
 * 
 * @param rng Генератор случайных чисел.
 * @param len Длина строки.
 * @return std::string Сгенерированная строка.
 */
std::string randomString(std::mt19937& rng, size_t len) {
    std::uniform_int_distribution<int> dist('a', 'z');
    std::string s(len, '\0');
    for (char& c : s) c = static_cast<char>(dist(rng));
    return s;
}

/**
 * @brief Создаёт вектор пассажиров с гарантированными дубликатами ключей.
 * 
 * @param n Количество пассажиров.
 * @param rng Генератор случайных чисел.
 * @return std::vector<Passenger> Вектор пассажиров.
 */
std::vector<Passenger> makeData(size_t n, std::mt19937& rng) {
    // 6.1 Ограниченный пул имён (~5 % от n)
    size_t uniqueCnt = std::max<size_t>(10, n / 20);
    std::vector<std::string> pool;
    pool.reserve(uniqueCnt);
    for (size_t i = 0; i < uniqueCnt; ++i) pool.push_back(randomString(rng, 10));

    std::uniform_int_distribution<int> cabinNum(1, 1000);
    static const std::string types[] {"Lux", "1", "2", "3"};
    std::uniform_int_distribution<int> typeDist(0, 3);

    std::vector<Passenger> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        Passenger p;
        p.fullName = pool[rng() % pool.size()];          // дубликаты гарантированы
        p.cabinNumber = cabinNum(rng);
        p.cabinType = types[typeDist(rng)];
        p.destinationPort = randomString(rng, 6);
        v.push_back(std::move(p));
    }
    return v;
}

/**
 * @struct ResultRow
 * @brief Структура для хранения результатов измерений времени и коллизий.
 */
struct ResultRow {
    size_t size;            /**< Размер данных */
    double tLinear;         /**< Время линейного поиска, нс */
    double tBST;            /**< Время поиска в BST, нс */
    double tRBT;            /**< Время поиска в красно-чёрном дереве, нс */
    double tHash;           /**< Время поиска в хеш-таблице, нс */
    double tMultimap;       /**< Время поиска в std::multimap, нс */
    size_t collisions;      /**< Количество коллизий хеш-таблицы */
};

/**
 * @brief Функция для измерения времени выполнения переданной функции.
 * 
 * @tparam F Тип callable объекта (лямбда, функция и т.п.).
 * @param f Объект функции для измерения.
 * @return double Время выполнения в наносекундах.
 */
template<typename F>
static double timeIt(F&& f) {
    auto beg = std::chrono::steady_clock::now();
    f();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - beg).count();
}

/**
 * @brief Точка входа программы.
 * 
 * Генерирует данные разного размера, строит все структуры,
 * замеряет время поиска, сохраняет результаты в CSV.
 * 
 * @return int Код возврата (0 — успех).
 */
int main() {
    std::vector<size_t> sizes {
        100, 1'000, 5'000, 10'000, 50'000, 100'000,
        200'000, 500'000, 750'000, 1'000'000
    };
    std::mt19937 rng(std::random_device{}());

    std::vector<ResultRow> rows;

    for (size_t n : sizes) {

        auto data = makeData(n, rng);
        std::uniform_int_distribution<size_t> pick(0, n - 1);
        std::string key = data[pick(rng)].fullName; // гарантированно существует

        double tLin = timeIt([&] { linearSearch(data, key); });

        BST bst;
        for (const auto& p : data) bst.insert(p);
        double tBST = timeIt([&] { bst.search(key); });

        RBTree rbt;
        for (const auto& p : data) rbt.insert(p);
        double tRBT = timeIt([&] { rbt.search(key); });

        HashTable ht(n * 2 + 1);
        for (const auto& p : data) ht.insert(p);
        double tHash = timeIt([&] { ht.search(key); });
        size_t colls = ht.collisionCount();

        std::multimap<std::string, const Passenger*> mp;
        for (const auto& p : data) mp.emplace(p.fullName, &p);
        double tMulti = timeIt([&] { mp.equal_range(key); });

        rows.push_back({n, tLin, tBST, tRBT, tHash, tMulti, colls});
        std::cout << "N=" << n << " done\n";
    }
    std::ofstream csv("search_times.csv");
    csv << "size,linear_us,bst_us,rbt_us,hash_us,multimap_us,collisions\n";
    for (const auto& r : rows) {
        csv << r.size << ',' << r.tLinear << ',' << r.tBST << ','
            << r.tRBT << ',' << r.tHash << ',' << r.tMultimap << ','
            << r.collisions << "\n";
    }
    std::cout << "Результаты сохранены в search_times.csv\n";
    return 0;
}

