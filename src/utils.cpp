#include "utils.h"
namespace utils {
	int roundUp(int numToRound, int multiple)
	{
		if(multiple == 0) {  
			return numToRound;  
		}  
		int remainder = numToRound % multiple; 
		if (remainder == 0) {
			return numToRound; 
		}
		int x;
		return numToRound + multiple - remainder; 
	}
}
