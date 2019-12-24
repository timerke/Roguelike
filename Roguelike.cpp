#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <vector>

// Глобальный тип Object_type для переменных, которые:
// empty_ - для пустых объектов;
// entrance_ - для Входа на уровень, или путь на предыдущий уровень;
// entrance_hero_ - для Входа на уровень вместе с Героем;
// exit_ - для Выхода из уровня, или путь на следующий уровень;
// exit_hero_ - для Выхода из уровня вместе с Героем;
// volcano_ - для Вулкана, куда Герою нужно бросить кольцо;
// food_ - для объекта, представляющего собой Еду;
// weapon_ - для объекта, представляющего собой Оружие;
// ork_ - для Орка, который является противником Героя;
// hero_ - для Героя, за которого играет игрок
enum Object_type { empty_, entrance_, entrance_hero_, exit_, exit_hero_, volcano_, food_, weapon_, orc_, hero_ };

// Глобальный тип Status для переменных, которые показывают статус в игре:
// waiting_ - ожидание игры;
// winning_ - игрок победил;
// losing_ - игрок проиграл;
// playing_ - игрок играет
enum Status { waiting_, winning_, losing_, playing_ };

// Глобальный тип Transition для переменных, которых показывают, нужно ли сменить уровень в игре:
// nowhere_ - уровень менять не нужно;
// next_level_ - нужно перейти на следующий уровень;
// previous_level_ - нужно перейти на предыдущий уровень
enum Transition { nowhere_, next_level_, previous_level_ };

const int MAX_LEVEL_NUM = 10; // максимальное возможное количество уровней в игре
const int MIN_LEVEL_NUM = 1; // минимальное возможное количество уровней в игре
const int MAX_SIZE = 15; // максимальный возможный размер карты
const int MIN_SIZE = 5; // минимальный возможный размер карты
const int MAX_HEALTH = 100; // максимальное возможное начальное здоровье Орка и начальное здоровье Героя
const int MIN_HEALTH = 10; // минимальное возможное начальное здоровье Орка
const int MAX_WEAPON_POWER = 60; // максимальная сила Оружия
const int MIN_WEAPON_POWER = 20; // минимальная сила Оружия
const int MAX_FOOD_NUM = 5; // максимальное возможное количество Еды на карте
const int MIN_FOOD_NUM = 0; // минимальное возможное количество Еды на карте
const int MAX_WEAPON_NUM = 5; // максимальное возможное количество Оружия на карте
const int MIN_WEAPON_NUM = 0; // минимальное возможное количество Оружия на карте
const int MAX_ORC_NUM = 6; // максимальное возможное количество Орков на карте
const int MIN_ORC_NUM = 2; // минимальное возможное количество Орков на карте

// Общий родительский класс Object для всех объектов в игре
class Object
{
public:
	int index; // индекс объекта в массиве таких же объектов на карте
	Object_type object_type; // тип объекта класса Object; может принимать одно из значений: entrance_, exit_, meal_, weapon_, ork_, hero_
	// Конструктор класса
	Object(Object_type object_type = empty_, int index = 0)
	{
		this->index = index;
		this->object_type = object_type;
	}
};
// Перегрузка оператора == для класса Object
bool operator == (Object o1, Object o2)
{
	return (o1.index == o2.index && o1.object_type == o2.object_type);
}
// Перегрузка оператора != для класса Object
bool operator != (Object o1, Object o2)
{
	return (o1.index != o2.index || o1.object_type != o2.object_type);
}

// Класс Food - класс для объектов, представляющих Eду; это дочерний класс класса Object
class Food : public Object
{
	// Объекты класса Еда могут быть трех видов:
	// 1. бифштекс (steak) с питательностью 60;
	// 2. хлеб (bread) с питательностью 40;
	// 3. картошка (potato) с питательностью 20
	static int values[]; // массив из значений питательности Еды; значение питательности добавляется к здоровью Героя
	static std::string names[]; // массив из названий Еды
private:
	int i_food; // индекс Еды из списка возможных типов Еды
public:
	// Конструктор класса
	Food(int i) : Object(food_)
	{
		this->i_food = i;
	}
	// Метод для сообщения информации о Еде
	void info()
	{
		std::cout << names[i_food] << " (food value = " << values[i_food] << ")";
	}
	// Метод, возвращающий питательность Еды
	int value()
	{
		return values[i_food];
	}
};
int Food::values[] = { 20, 40, 60 }; // определение статического массива с питательностью Еды
std::string Food::names[] = { "potato", "bread", "steak" }; // определение статического массива с названиями Еды
const int FOODS_NUM = 3; // количество разных видов Еды

// Класс Weapon - класс для объектов, представляющих Оружие; это дочерний класс класса Object
class Weapon : public Object
{
	// Объекты класса Оружие характеризуются силой. Если Персонаж собирает несколько Оружий, их силы складываются
	// и остается одно Оружие. В сражении сила Оружия спадает
	friend class Character; // у класса Weapon есть дружественный класс Character
	friend class Game; // у класса Weapon есть дружественный класс Game
private:
	int power; // сила оружия
public:
	// Конструктор класса
	Weapon(int power = 0) : Object(weapon_)
	{
		this->power = power;
	}
	// Метод увеличивает силу Оружия
	Weapon& operator += (Weapon weapon)
	{
		this->power += weapon.power;
		return *this;
	}
};

// Класс Character - класс для Персонажей игры (Орков и Героя); это дочерний класс класса Object
class Character : public Object
{
	friend class Game; // у класса Character есть дружественный класс Game
protected:
	int health; // здоровье Персонажа
	Weapon weapon; // Оружие, которое есть у Персонажа
public:
	// Конструктор класса
	Character(int health = MAX_HEALTH, Object_type object_type = hero_) : Object(object_type)
	{
		this->health = health; // начальное здоровье Персонажа
	}
	// Метод показывает полную информацию о Персонаже
	void info()
	{
		std::string name = object_type == hero_ ? "Hero" : "Orc"; // кем является Персонаж - Орком или Героем
		std::cout << name << "'s health = " << health << ". The " << name << " has weapon with power " << weapon.power;
	}
	// Метод, чтобы взять найденное Персонажем Оружие
	void take_weapon(Weapon& new_weapon, bool is_text_required = false)
	{
		weapon += new_weapon;
		if (is_text_required)
		{
			// Если нужно вывести информацию о выполнении метода
			std::cout << "\tThe Hero found and took weapon with power " << new_weapon.power << ".\n";
		}
	}
	// Метод для питания Персонажа
	void eat(Food* food)
	{
		health += food->value();
		std::cout << "\tThe Hero found and ate the ";
		food->info();
		std::cout << ". Hero's health = " << health << ".\n";
	}
	// Метод для нападения на Противника
	void fight(Character* opponent)
	{
		if (weapon.power == MIN_WEAPON_POWER)
		{
			// Если сила оружия нападающего минимально возможная
			opponent->health -= weapon.power;
		}
		else if (MIN_WEAPON_POWER < weapon.power && weapon.power <= opponent->health)
		{
			// Если сила оружия нападающего больше минимального значения, но меньше здоровья его противника
			opponent->health -= weapon.power; // здоровье противника уменьшается
			weapon.power = MIN_WEAPON_POWER; // сила оружия нападающего становится минимально возможной
		}
		else if (MIN_WEAPON_POWER < weapon.power && opponent->health < weapon.power)
		{
			// Если сила оружия нападающего больше минимального значения и здоровья противника, то
			// сила оружия уменьшается на величину здоровья противника, а если при этом сила оружия становится
			// меньше минимального возможного значения, то сила оружия принимает минимально возможное значение
			weapon.power -= opponent->health;
			weapon.power = weapon.power < MIN_WEAPON_POWER ? MIN_WEAPON_POWER : weapon.power;
			opponent->health = 0; // здоровье противника уменьшается до 0
		}
	}
};

// Класс Level - класс с описанием уровня игры
class Level
{
	friend class Game; // у класса Level есть дружественный класс Game
protected:
	Transition trans; // атрибут отвечает за переход с уровня на уровень
	int size_x, size_y; // размеры карты по горизонтали и вертикали
	int x, y; // положение Героя
	int in_out[2][2]; // координаты Входа и Выхода из уровня
	Object** map; // карта уровня
	std::vector<Food> foods; // вектор с Едой на карте
	std::vector<Weapon> weapons; // вектор с Оружием на карте
	std::vector<Character> orcs; // вектор с Орками на карте
	std::vector<Object*> foods_weapons_around; // вектор с индексами Еды и Оружия рядом с Героем
	std::vector<Object*> orcs_around; // вектор с индексами Орков рядом с Героем
public:
	// Конструктор класса
	Level(bool is_last = false)
	{
		trans = nowhere_; // Герой на уровне и никуда не переходит
		// Случайным образом создается карта уровня
		size_x = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1); // горизонтальный размер карты уровня
		size_y = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1); // вертикальный размер карты уровня
		// Строится пустая карта уровня
		map = new Object * [size_y];
		for (int iy = 0; iy < size_y; iy++)
		{
			map[iy] = new Object[size_x];
			for (int ix = 0; ix < size_x; ix++)
			{
				map[iy][ix] = Object(empty_);
			}
		}
		// На карту помещается Вход и туда же Герой
		int temp = select_position(entrance_hero_);
		x = temp % size_x; // определяется горизонтальная координата Героя
		y = temp / size_x; // определяется вертикальная координата Героя
		in_out[0][0] = x; // записывается горизонтальная координата Входа
		in_out[0][1] = y; // записывается вертикальная координата Входа
		// Последний или непоследний уровень в игре
		if (is_last)
		{
			// На карту помещается Вулкан, если создается последний уровень
			temp = select_position(volcano_);
			in_out[1][0] = temp % size_x; // записывается горизонтальная координата Вулкана
			in_out[1][1] = temp / size_y; // записывается вертикальная координата Вулкана
		}
		else
		{
			// На карту помещается Выход, если создается непоследний уровень
			temp = select_position(exit_);
			in_out[1][0] = temp % size_x; // записывается горизонтальная координата Выхода
			in_out[1][1] = temp / size_y; // записывается вертикальная координата Выхода
		}
		// На карту помещается Еда
		int foods_num = MIN_FOOD_NUM + rand() % (MAX_FOOD_NUM - MIN_FOOD_NUM + 1); // количество Еды на карте
		for (int i = 0; i < foods_num; i++)
		{
			int food_type = rand() % FOODS_NUM; // вид Еды определяется случайным образом
			foods.push_back(Food(food_type)); // Еда помещается в вектор с Едой на карте
			select_position(food_, i);
		}
		// На карту помещается Оружие
		int weapons_num = MIN_WEAPON_NUM + rand() % (MAX_WEAPON_NUM - MIN_WEAPON_NUM + 1); // количество Оружия на карте
		for (int i = 0; i < weapons_num; i++)
		{
			int weapon_power = MIN_WEAPON_POWER + rand() % (MAX_WEAPON_POWER - MIN_WEAPON_POWER + 1); // сила Оружия определяется случайным образом
			weapons.push_back(Weapon(weapon_power)); // Оружие помещается в вектор с Оружием на карте
			select_position(weapon_, i);
		}
		// На карту помещаются Орки
		int orcs_num = MIN_ORC_NUM + rand() % (MAX_ORC_NUM - MIN_ORC_NUM + 1); // количество Орков на карте
		for (int i = 0; i < orcs_num; i++)
		{
			int orc_health = MIN_HEALTH + rand() % (MAX_HEALTH - MIN_HEALTH + 1); // случайное значение здоровья Орка
			Character orc(orc_health, orc_); // создается Орк
			int weapon_power = MIN_WEAPON_POWER + rand() % (MAX_WEAPON_POWER - MIN_WEAPON_POWER + 1); // сила Оружия для Орка
			Weapon weapon(weapon_power); // создается Оружие для Орка
			orc.take_weapon(weapon); // Орк получает Оружие
			orcs.push_back(orc); // Орк помещается в вектор с Орками на карте
			select_position(orc_, i);
		}
	}
	// Метод случайным образом помещает объект на карту
	int select_position(Object_type object_type, int index = 0)
	{
		int i = rand() % (size_x * size_y); // случайным образом выбирается номер клетки на карте
		// Ищется свободная клетка на карте
		int ix = -1, iy = 0, j = -1;
		while (j < i)
		{
			if (size_x <= ++ix)
			{
				ix = 0;
				if (size_y <= ++iy)
				{
					iy = 0;
				}
			}
			if (map[iy][ix].object_type == empty_)
			{
				j++;
			}
		}
		// В выбранную клетку помещается объект
		map[iy][ix] = Object(object_type, index);
		return iy * size_x + ix; // возвращается номер клетки, в которую помещен объект
	}
	// Метод рисует карту
	void draw()
	{
		std::cout << "\t+";
		for (int ix = 0; ix < this->size_x; ix++)
		{
			std::cout << "-";
		}
		std::cout << "+\n";
		for (int iy = 0; iy < size_y; iy++)
		{
			std::cout << "\t|";
			for (int ix = 0; ix < size_x; ix++)
			{
				if (map[iy][ix].object_type == entrance_)
				{
					std::cout << "&";
				}
				else if (map[iy][ix].object_type == exit_)
				{
					std::cout << "#";
				}
				else if (map[iy][ix].object_type == volcano_)
				{
					std::cout << "*";
				}
				else if (map[iy][ix].object_type == food_)
				{
					std::cout << "%";
				}
				else if (map[iy][ix].object_type == weapon_)
				{
					std::cout << "!";
				}
				else if (map[iy][ix].object_type == orc_)
				{
					std::cout << "O";
				}
				else if (map[iy][ix].object_type == hero_ || map[iy][ix].object_type == entrance_hero_ || map[iy][ix].object_type == exit_hero_)
				{
					std::cout << "@";
				}
				else
				{
					std::cout << " ";
				}
			}
			std::cout << "|\n";
		}
		std::cout << "\t+";
		for (int ix = 0; ix < size_x; ix++)
		{
			std::cout << "-";
		}
		std::cout << "+\n";
	}
	// Метод находит объекты вокруг Героя
	bool around(int x_, int y_)
	{
		orcs_around.clear(); // очищается вектор с Орками вокруг Героя
		foods_weapons_around.clear(); // очищается вектор с Едой и Оружием вокруг Героя
		// Проверяется, можно ли Герою находится в точке x_, y_
		if (x_ < 0 || size_x <= x_ || y_ < 0 || size_y <= y_)
		{
			// Если координаты x_, y_ оказываются вне карты, Герой не может находиться в этой точке
			return false;
		}
		else if (map[y_][x_].object_type == orc_)
		{
			// Если на координате x_, y_ находится Орк, Герой не может находиться в этой точке
			return false;
		}
		// Если Герой может находиться в точке x_, y_
		for (int ix = x_ - 1; ix < x_ + 2; ix++)
		{
			if (-1 < ix && ix < size_x)
			{
				for (int iy = y_ - 1; iy < y_ + 2; iy++)
				{
					if (-1 < iy && iy < size_y && (ix != x || iy != y))
					{
						if (map[iy][ix].object_type == orc_)
						{
							// Если рядом с Героем есть Орк
							orcs_around.push_back(&map[iy][ix]);
						}
						else if (map[iy][ix].object_type == food_ || map[iy][ix].object_type == weapon_)
						{
							// Если рядом с Героем есть Еда или Оружие
							foods_weapons_around.push_back(&map[iy][ix]);
						}
					}
				}
			}
		}
		return true;
	}
	// Метод передвигает Героя по карте
	Transition move(char direction, Character* hero)
	{
		int x_ = x, y_ = y;
		if (direction == 'u')
		{
			// Движение вверх
			y_--;
		}
		else if (direction == 'd')
		{
			// Движение вниз
			y_++;
		}
		else if (direction == 'l')
		{
			// Движение влево
			x_--;
		}
		else if (direction == 'r')
		{
			// Движение вправо
			x_++;
		}
		if (around(x_, y_))
		{
			// Если Герой может пройти в нужном направлении
			if (map[y][x].object_type == entrance_hero_)
			{
				// Если в предыдущем положении Героя находился Вход
				map[y][x].object_type = entrance_;
			}
			else if (map[y][x].object_type == exit_hero_)
			{
				// Если в предыдущем положении Героя находился Выход
				map[y][x].object_type = exit_;
			}
			else
			{
				// В ином случае
				map[y][x].object_type = empty_;
			}
			// Если Герой по дороге нашел Еду и/или Оружие
			for (auto iter = foods_weapons_around.begin(); iter < foods_weapons_around.end(); iter++)
			{
				if ((*iter)->object_type == food_)
				{
					// Если Герой нашел Еду
					hero->eat(&foods[(*iter)->index]); // Герой ест Еду
				}
				else
				{
					// Если Герой нашел Оружие
					hero->take_weapon(weapons[(*iter)->index], true); // Герой берет Оружие
				}
				(*iter)->object_type = empty_; // Еда или Оружие удаляются с карты map
			}
			x = x_; // горизонтальной координате Героя присваивается новое значение
			y = y_; // вертикальной координате Героя присваивается новое значение
			if (map[y][x].object_type == entrance_)
			{
				// Если новое положение Героя совпадает со Входом
				map[y][x].object_type = entrance_hero_;
				return previous_level_; // нужно перейти на предыдущий уровень
			}
			else if (map[y][x].object_type == exit_)
			{
				// Если новое положение Героя совпадает с Выходом
				map[y][x].object_type = exit_hero_;
				return next_level_; // нужно перейти на следующий уровень
			}
			else if (map[y][x].object_type == volcano_)
			{
				// Если новое положение Героя совпадает с Вулканом
				map[y][x].object_type = hero_;
				return next_level_; // нужно перейти на следующий уровень
			}
			else
			{
				// Иначе
				map[y][x].object_type = hero_;
				return nowhere_; // на новый уровень переходить не надо
			}
		}
		return nowhere_;
	}
	// Метод рассказывает об Орках рядом с Героем
	void show_orcs_around()
	{
		if (orcs_around.size())
		{
			// Если рядом с Героем есть Орки
			std::cout << "\tThere are Orcs around the Hero:\n";
			int i = 0;
			for (auto iter = orcs_around.begin(); iter < orcs_around.end(); iter++)
			{
				std::cout << "\t\t" << ++i << ". ";
				orcs[(*iter)->index].info();
				std::cout << ".\n";
			}
		}
	}
};

// Класс Game - класс с описанием игры
class Game
{
private:
	Character hero; // Герой - персонаж, за которого играет пользователь
	Transition trans; // параметр отвечает за переход с уровня на уровень
	int x, y; // горизонтальная и вертикальная координата Героя
	std::vector<Level> levels; // вектор с картами уровней
public:
	int level_num; // количество уровней
	int level; // уровень, на котором находится игрок
	Status status; // статус в игре
	// Конструктор класса
	Game()
	{
		status = waiting_; // статус игры - игра ожидается
		trans = nowhere_; // Герой на уровне и никуда не переходит
		// Случайным образом создается карта уровней
		level = 0; // начальный уровень игры
		level_num = MIN_LEVEL_NUM + rand() % (MAX_LEVEL_NUM - MIN_LEVEL_NUM + 1); // количество уровней в игре
		for (int i = 0; i < level_num; i++)
		{
			bool is_last = (i == level_num - 1) ? true : false; // если последний уровень, то true
			levels.push_back(Level(is_last)); // создаются карты уровней
		}
		Weapon weapon(MIN_WEAPON_POWER); // создается Оружие с минимальной силой для Героя
		hero.take_weapon(weapon); // Герой получает Оружие с минамальной силой
	}
	// Метод показывает карту уровня, на котором находится игрок
	void draw()
	{
		std::cout << "\tMap of level " << level + 1 << " is...\n";
		levels[level].draw();
	}
	// Метод выполняет действия при начале игры
	void start()
	{
		status = playing_; // меняется статус игры
		this->x = levels[level].x; // читается горизонтальная координата начального положения Героя
		this->y = levels[level].y; // читается вертикальная координата начального положения Героя
		levels[level].around(x, y); // определяются объекты вокруг Героя
		levels[level].show_orcs_around(); // показываются характеристики Орков рядом с Героем
	}
	// Метод показывает полную информацию о Герое
	void info()
	{
		hero.info();
		std::cout << ".\n";
	}
	// Метод передвигает Героя по карте
	void move(char direction)
	{
		trans = levels[level].move(direction, &hero); // Герой передвинут в нужном направлении
		// Анализируется состояния игры и Героя
		if (hero.health <= 0)
		{
			// Если Герой потерял все здоровье, то проиграл
			status = losing_;
		}
		else if (trans == next_level_ && level + 1 == level_num)
		{
			// Если Герой дошел до Вулкана, то победил
			status = winning_;
		}
		else if (trans == next_level_ && level + 1 != level_num)
		{
			// Герой переходит на следующий уровень
			status = playing_;
			level++;
			std::cout << "\tLEVEL " << level + 1 << "\n";
		}
		else if (trans == previous_level_ && 0 < level)
		{
			// Герой переходит на предыдущий уровень
			status = playing_;
			level--;
			std::cout << "\tLEVEL " << level + 1 << "\n";
		}
		this->x = levels[level].x; // читается горизонтальная координата положения Героя
		this->y = levels[level].y; // читается вертикальная координата положения Героя
		levels[level].around(x, y);
		levels[level].show_orcs_around(); // показываются характеристики Орков рядом с Героем
		if (status == playing_)
		{
			orc_fight(); // право выполнить действие передается Оркам
		}
		if (hero.health <= 0)
		{
			// Если Герой потерял все здоровье, то проиграл
			status = losing_;
		}
	}
	// Метод для борьбы Героя с Орками; в качестве аргумента передается номер орка
	void hero_fight(int i_orc)
	{
		if (-1 < i_orc && i_orc < levels[level].orcs_around.size())
		{
			auto iter = levels[level].orcs_around.begin() + i_orc; // определяется ссылка на объект на карте, который и есть нужный Орк
			hero.fight(&levels[level].orcs[(*iter)->index]); // Герой наносит удары Орку
			if (levels[level].orcs[(*iter)->index].health <= 0)
			{
				// Если Орк потерял все здоровье, он удаляется с карты
				levels[level].orcs_around[i_orc]->object_type = empty_;
				levels[level].around(x, y);
			}
		}
		levels[level].show_orcs_around(); // показываются характеристики Орков рядом с Героем
		if (status == playing_)
		{
			orc_fight(); // право выполнить действие передается Оркам
		}
		if (hero.health <= 0)
		{
			// Если Герой потерял все здоровье, то проиграл
			status = losing_;
		}
	}
	// Метод для борьбы Орков с Героем
	void orc_fight()
	{
		if (levels[level].orcs_around.size())
		{
			// Если рядом с Героем есть Орки, найдем среди них Орка с наибольшей силой Оружия
			int max_power = 0; // наибольшая сила оружия Орка
			int i_orc; // номер Орка в списке
			for (auto iter = levels[level].orcs_around.begin(); iter < levels[level].orcs_around.end(); iter++)
			{
				auto iter_ = levels[level].orcs.begin() + (*iter)->index;
				if (max_power < (*iter_).weapon.power)
				{
					max_power = (*iter_).weapon.power;
					i_orc = (*iter)->index;
				}
			}
			// Теперь Орк с наибольшей силой Оружия наносит удары Герою
			auto iter = levels[level].orcs.begin() + i_orc;
			(*iter).fight(&hero);
			std::cout << "\tThe Orc (";
			(*iter).info();
			std::cout << ") battles the Hero.\n";
		}
	}
};

int main()
{
	srand((unsigned int)time(0));
	Game game;
	std::cout << "\n\tWELCOME TO 'TRIP TO THE VOLCANO'\n";
	std::cout << "\tLet's start the game! You can always call for help by pressing '?'.\n";
	while (true)
	{
		char user_answer[100];
		std::cout << "Your command: ";
		std::cin.getline(user_answer, 100);
		if (user_answer[0] == '?')
		{
			// Выводится справочная информация по игре
			std::cout << "\tHELP\n";
			std::cout << "\t1. You can press '?' to call help.\n";
			std::cout << "\t2. To start the game click 's' or 'S'.\n";
			std::cout << "\t\t2.1. The Hero is indicated on the map as '@'.\n";
			std::cout << "\t\t2.2. The Hero has a ring. The Hero must reach the last level, find the mouth of the volcano and\n\t\tthrow the ring at it.\n";
			std::cout << "\t\t2.3. The volcano is at the last level of the game and is represented on the map as '*'.\n";
			std::cout << "\t\t2.4. The Hero can move vertically and horizontally. Press 'u' or 'U' to move up and 'd' or 'D' to\n\t\tmove down. Press 'l' or 'L' to move left and 'r' or 'R' to move right.\n";
			std::cout << "\t\t2.5. The Hero must get to the exit to go to the next level. The exit is indicated on the map as '#'.\n";
			std::cout << "\t\t2.6. The Hero must get to the entrance to go to the previous level. The entrance is indicated on\n\t\tthe map as '&'.\n";
			std::cout << "\t\t2.7. There are Orcs on the levels. If the Hero approaches the Orc, the Orc may attack and kill\n\t\tthe Hero. The Orcs are denoted on the map as 'O'.\n";
			std::cout << "\t\t2.8. The Hero can fight the Orc by clicking 'b' or 'B'. The Hero can increase the power of\n\t\tweapon by finding weapons on the map. The weapons are denoted on the map as '!'.\n";
			std::cout << "\t\t2.9. The Hero can see Hero's weapon by clicking 'i' or 'I'.\n";
			std::cout << "\t\t2.10. The Hero must eat food to restore health. The food is denoted on the map as '%'.\n";
			std::cout << "\t3. To finish the game click 'q' or 'Q'.\n";
		}
		else if (user_answer[0] == 's' || user_answer[0] == 'S')
		{
			// Старт игры
			std::cout << "\tLEVEL 1";
			Game new_game;
			game = new_game;
			game.start();
			std::cout << "\n";
			game.draw(); // показывается карта уровня
		}
		else if (user_answer[0] == 'q' || user_answer[0] == 'Q')
		{
			// Выход из игры
			std::cout << "\tGAME IS FINISHED! GOODBYE!";
			break;
		}
		else if (game.status == playing_)
		{
			if (user_answer[0] == 'i' || user_answer[0] == 'I')
			{
				// Показывается информация о Герое
				std::cout << "\tLevel " << game.level + 1 << ". ";
				game.info();
			}
			else if (user_answer[0] == 'u' || user_answer[0] == 'U')
			{
				// Если Герой должен двигаться вверх
				game.move('u');
				game.draw(); // показывается карта уровня
			}
			else if (user_answer[0] == 'd' || user_answer[0] == 'D')
			{
				// Если Герой должен двигаться вниз
				game.move('d');
				game.draw(); // показывается карта уровня
			}
			else if (user_answer[0] == 'l' || user_answer[0] == 'L')
			{
				// Если Герой должен двигаться влево
				game.move('l');
				game.draw(); // показывается карта уровня
			}
			else if (user_answer[0] == 'r' || user_answer[0] == 'R')
			{
				// Если Герой должен двигаться вправо
				game.move('r');
				game.draw(); // показывается карта уровня
			}
			else if (user_answer[0] == 'b' || user_answer[0] == 'B')
			{
				// Если Герой сражается с Орком
				std::cout << "\tEnter number of orc: ";
				std::cin.getline(user_answer, 100);
				int i = 0, j;
				if ('0' <= user_answer[0] && user_answer[0] <= '9')
				{
					j = (int)'0';
					i = (int)user_answer[0] - j - 1;
				}
				game.hero_fight(i);
				game.draw();
			}
			// Проверка на статус игры
			if (game.status == winning_)
			{
				// Если Герой победил
				std::cout << "\t!!!YOU WON!!!\n";
				game.status = waiting_;
			}
			else if (game.status == losing_)
			{
				// Если Герой проиграл
				std::cout << "\t***YOU LOSE***\n";
				game.status = waiting_;
			}
		}
	}
	return 0;
}