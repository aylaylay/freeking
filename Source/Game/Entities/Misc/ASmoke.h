#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Misc
{
    class ASmoke : public BaseEntity
    {
    public:

        ASmoke();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
