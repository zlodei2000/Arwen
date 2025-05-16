//
#ifndef	__ENTITIES__
#define	__ENTITIES__

#include	"Array.h"
#include	"ObjStr.h"

class	Entity : public Object
{
private:
	String	tag;
	String	value;
public:
	Entity ( const String& theTag, const String& theValue ) : Object ( theTag ), tag ( theTag ), value ( theValue ) {}

    virtual const char  * getClassName () const
    {
        return "Entity";
    }

    virtual bool isOk () const           // returns non-zero if objects is ok
    {
        return tag.isOk () && value.isOk ();
    }

                                        // returns  non-zero if this object is
                                        // equal to the object obj
    virtual int compare ( const Object * obj ) const
	{
		return tag.compare ( obj );
	}

	const String&	getTag () const
	{
		return tag;
	}

	const	String&	getValue () const
	{
		return value;
	}
};

class	EntityGroup : public Object
{
private:
	Array	entities;
public:
	EntityGroup ( char * text );

    virtual const char  * getClassName () const
    {
        return "EntityGroup";
    }

    virtual bool isOk () const           // returns non-zero if objects is ok
    {
        return entities.isOk ();
    }

	void			addEntity ( const String& tag, const String& value );
	const String	getValue  ( const String& tag ) const;

	int	getNumEntites () const
	{
		return entities.getCount ();
	}

	Entity * getEntity ( int index ) const
	{
		return (Entity *) entities.at ( index );
	}
};

class	Entities : public Object
{
private:
	Array	groups;
public:
	Entities ( char * text );

    virtual const char  * getClassName () const
    {
        return "Entities";
    }

    virtual bool isOk () const           // returns non-zero if objects is ok
    {
        return groups.isOk ();
    }

	int	getNumGroups () const
	{
		return groups.getCount ();
	}

	EntityGroup * getGroup ( int index ) const
	{
		if ( index < 0 || index >= groups.getCount () )
			return NULL;

		return (EntityGroup *) groups.at ( index );
	}
};

#endif
