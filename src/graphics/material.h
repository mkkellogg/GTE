#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "shader/shader.h"
#include "attributes.h"
#include "graphics.h"

class Material
{
	friend class Graphics;

	static const int VAR_BINDINGS_SIZE=64;

	Shader * shader;
	int varBindings[VAR_BINDINGS_SIZE];

	void BindVars();
	void ClearBindings();
	void SetBinding(int location, Attribute attr);
	int AttributeMaskToIndex(AttributeMask mask);
	int TestForAttribue(Attribute attr);

    protected:

    Material();
    virtual ~Material();
    void Init(Shader * shader);

    public:

};

#endif
