#pragma once

#include "IEntity.h"

namespace Freeking::Entity::Target
{
    class ACrosslevelTrigger : public BaseEntity
    {
    public:

        ACrosslevelTrigger();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}