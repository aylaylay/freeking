#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Props2
{
    class AFishSpawnerNode : public BaseEntity
    {
    public:

        AFishSpawnerNode();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
