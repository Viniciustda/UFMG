import Call from "../entity/Call";
import database from "../infra/database";

export default class CallRepository {

    async getAll(): Promise<Call[]>  {
        const callsData = await database.manyOrNone("select c.* from postgres.escall.call as c");
        if (!callsData) {
            return [];
        }
        const calls = callsData.map((callData) => {
            const call =  new Call(callData.id,
                callData.name,
                callData.phone_data,
                callData.description,
                callData.status,
                callData.priority,
                callData.created_at
            );
            call.setFinished(callData.finished_at);
            call.setTechnicianAddedDate(callData.technician_added_date);
            call.setSupportTechnicianId(callData.support_technician_id);

            return call;
        })
        return calls;
    }

    async addSupportTechnician(
        callId: number,
        supportTechnicianId: number,
        technicianAddedDate: Date,
        status: string
    ): Promise<void> {
        await database.none("update postgres.escall.call set support_technician_id = $1, technician_added_date = $2, status = $3 where id = $4",
            [
                supportTechnicianId,
                technicianAddedDate,
                status,
                callId
            ]
        );
    }

    async setPriority(callId: number, priority: string): Promise<void> {
        await database.none("update postgres.escall.call set priority = $1 where id = $2",
            [
                priority,
                callId
            ]
        );
    }

    async setStatus(callId: number, status: string): Promise<void> {
        await database.none("update postgres.escall.call set status = $1, finished_at = $2 where id = $3",
            [
                status,
                new Date(),
                callId
            ]
        );
    }

    private async nextId(): Promise<number> {
        const maxId = await database.one("select max(id) as max_id from postgres.escall.call");
        return maxId.max_id + 1; 
    }

    async create(call: Call): Promise<void> {
        const nextId = await this.nextId();

        await database.none("insert into postgres.escall.call (id, name, phone_number, description, status, created_at, priority) values ($1, $2, $3, $4, $5, $6, $7)",
            [
                nextId,
                call.getName(),
                call.getPhoneNumber(),
                call.getDescription(),
                call.getStatus(),
                call.getCreatedAt(),
                call.getPriority()
            ]
        );
    }

    async getAllBySupportTechnician(supportTechnicianId: number): Promise<Call[]> {
        const callsData = await database.manyOrNone("select c.* from postgres.escall.call as c where c.support_technician_id = $1",[supportTechnicianId]);
        if (!callsData) {
            return [];
        }
        const calls = callsData.map((callData) => {
            const call =  new Call(callData.id,
                callData.name,
                callData.phone_data,
                callData.description,
                callData.status,
                callData.priority,
                callData.created_at
            );
            call.setFinished(callData.finished_at);
            call.setTechnicianAddedDate(callData.technician_added_date);
            call.setSupportTechnicianId(callData.support_technician_id);

            return call;
        })
        return calls;
    }

}
