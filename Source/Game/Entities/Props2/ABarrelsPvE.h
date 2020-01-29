#pragma once

#include "IEntity.h"

namespace Freeking::Entity::Props2
{
    class ABarrelsPvE : public BaseEntity
    {
    public:

        ABarrelsPvE();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}