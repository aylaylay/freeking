#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Props
{
    class AExtinguishera : public BaseEntity
    {
    public:

        AExtinguishera();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityKeyValue& keyValue) override;

    private:

    };
}
