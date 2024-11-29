import SupportTechnician from "../entity/SupportTechnician";
import database from "../infra/database";

export default class SupportTechnicianRepository {

    async getAll(): Promise<SupportTechnician[]>  {        
        const supportTechniciansData = await database.manyOrNone("select st.id, st.name from postgres.escall.support_technician as st");
        if (!supportTechniciansData) {
            return [];
        }
        const supportTechnicians = supportTechniciansData.map((supportTechnicianData) => {
            return new SupportTechnician(supportTechnicianData.id, supportTechnicianData.name);
        })
        return supportTechnicians;
    }
}