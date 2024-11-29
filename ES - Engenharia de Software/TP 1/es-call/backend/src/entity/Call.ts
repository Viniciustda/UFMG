
export default class Call {

    private supportTechnicianId: number|null;
    private technicianAddedDate: Date|null;
    private finished: Date|null;
    private supportTechnicianName: string|null;

    constructor(
        private readonly id: number|null,
        private readonly name: string,
        private readonly phoneNumber: string,
        private readonly description: string,
        private readonly status: string,
        private readonly priority: string,
        private readonly createdAt: Date
    ) {
        this.supportTechnicianId = null;
        this.technicianAddedDate = null;
        this.finished = null;
        this.supportTechnicianName = null;
    }

    getId(): number|null {
        return this.id;
    }

    getName(): string {
        return this.name;
    }

    getPhoneNumber(): string {
        return this.phoneNumber;
    }

    getDescription(): string {
        return this.description;
    }

    getStatus(): string {
        return this.status;
    }

    getPriority(): string {
        return this.priority;
    }

    getCreatedAt(): Date {
        return this.createdAt;
    }

    setSupportTechnicianId(supportTechnicianId: number) {
        this.supportTechnicianId = supportTechnicianId;
    }
    getSupportTechnicianId(): number|null {
        return this.supportTechnicianId;
    }

    setTechnicianAddedDate(technicianAddedDate: Date) {
        this.technicianAddedDate = technicianAddedDate;
    }
    getTechnicianAddedDate(): Date|null {
        return this.technicianAddedDate;
    }

    setFinished(finished: Date) {
        this.finished = finished;
    }
    getFinished(): Date|null {
        return this.finished;
    }

    setSupportTechnicianName(supportTechnicianName: string) {
        this.supportTechnicianName = supportTechnicianName;
    }
    getSupportTechnicianName(): string|null {
        return this.supportTechnicianName;
    }

}
