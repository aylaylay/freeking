#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Target
{
    class ASpawner : public BaseEntity
    {
    public:

        ASpawner();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
