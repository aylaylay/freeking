#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Ai
{
    class ABoundary : public BaseEntity
    {
    public:

        ABoundary();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
