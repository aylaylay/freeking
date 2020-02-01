#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Trigger
{
    class ARelay : public BaseEntity
    {
    public:

        ARelay();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityProperty& property) override;

    private:

    };
}
