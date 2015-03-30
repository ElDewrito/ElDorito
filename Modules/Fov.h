#pragma	once

#include	"ElModule.h"

class	Fov : public	ElModule
{
public:
	Fov();
	~Fov();

	std::string	Info();

	void	Tick(const	std::chrono::duration<double>&	Delta);
	bool	Run(const	std::vector<std::string>&	Args);
private:
};