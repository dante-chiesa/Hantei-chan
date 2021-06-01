#include <string>

namespace fileType
{
enum {
	HA6,
	CG,
	PAL,
	TXT,
	PAT
};
}

std::string FileDialog(int fileType = -1, bool save = false);
