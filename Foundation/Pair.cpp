
#include	"Pair.h"

MetaClass	Pair :: classInstance ( "Pair", &Object :: classInstance );

int	Pair :: compare ( const Object * obj ) const
{
	int	res = (first != NULL ? first -> compare ( obj ) : 0);

	if ( res != 0 )
		return res;

	return (second != NULL ? second -> compare ( obj ) : 0);
}

