#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Light
{
    class AFireSm : public BaseEntity
    {
    public:

        AFireSm();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
