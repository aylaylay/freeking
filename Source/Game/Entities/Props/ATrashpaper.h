#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Props
{
    class ATrashpaper : public BaseEntity
    {
    public:

        ATrashpaper();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityProperty& property) override;

    private:

    };
}
