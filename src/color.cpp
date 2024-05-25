#include "color.h"

#include <vector>

// red1:red2:red3:red4:
// green1:green2:green3:green4:
// blue1:blue2:blue3:blue4:
// yellow1:yellow2:yellow3:yellow4:
// orange1:orange2:orange3:orange4:
// orangered:pink:purple:violet:magenta:maroon:gray
std::vector<std::string> colors{
    "red1",    "green1",  "blue1",  "yellow1", "orange1", "pink",    "gray20",  "red4",    "green4",  "blue4",
    "yellow4", "orange4", "purple", "gray60",  "red2",    "green2",  "blue2",   "yellow2", "orange2", "violet",
    "gray40",  "red3",    "green3", "blue3",   "yellow3", "orange3", "magenta", "gray80",  "maroon",
};

std::string getGraphVizColor(unsigned index)
{
  return colors[index % colors.size()];
}