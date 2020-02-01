#pragma once

#include "BaseEntity.h"

namespace Freeking::Entity::Props
{
    class ATableseta : public BaseEntity
    {
    public:

        ATableseta();

		virtual void Initialize() override;
		virtual void Tick(double dt) override;

	protected:

		virtual bool SetProperty(const EntityProperty& property) override;

    private:

    };
}
