#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Target
{
    class ASplash : public BaseEntity
    {
    public:

        ASplash();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityProperty& property) override;

    private:

    };
}
